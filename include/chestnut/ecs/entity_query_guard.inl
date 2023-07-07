#include "entity_query_guard.hpp"
namespace chestnut::ecs::internal
{    
    inline CEntityQueryGuard::CEntityQueryGuard(const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature)
    : m_requireSignature(requireSignature),
      m_rejectSignature(rejectSignature)
    {

    }

    inline const CEntitySignature &CEntityQueryGuard::requireSignature() const
    {
        return m_requireSignature;
    }

    inline const CEntitySignature &CEntityQueryGuard::rejectSignature() const
    {
        return m_rejectSignature;
    }

    inline const std::unordered_set<entityslot_t> &CEntityQueryGuard::pendingIn() const
    {
        return m_pendingIn_setEntitySlots;
    }

    inline const std::unordered_set<entityslot_t> &CEntityQueryGuard::pendingOut() const
    {
        return m_pendingOut_setEntitySlots;
    }

    inline void CEntityQueryGuard::enqueueEntity( entityslot_t entitySlot ) 
    {
        m_pendingOut_setEntitySlots.erase(entitySlot);
        m_pendingIn_setEntitySlots.insert(entitySlot);
    }

    inline void CEntityQueryGuard::dequeueEntity( entityslot_t entitySlot ) 
    {
        m_pendingIn_setEntitySlots.erase(entitySlot);
        m_pendingOut_setEntitySlots.insert(entitySlot);
    }

    inline bool CEntityQueryGuard::hasQueuedEntities() const
    {
        return !m_pendingIn_setEntitySlots.empty() || !m_pendingOut_setEntitySlots.empty();
    }

    inline SEntityQueryUpdateInfo CEntityQueryGuard::updateReceiver(std::vector<entityslot_t> &receiver)
    {
        SEntityQueryUpdateInfo updateInfo {0, 0, 0};

        // first do the removal
        if(!m_pendingOut_setEntitySlots.empty())
        {
            for(auto it = receiver.begin(); it != receiver.end(); /*NOP*/)
            {
                if(m_pendingOut_setEntitySlots.find(*it) != m_pendingOut_setEntitySlots.end())
                {
                    m_pendingOut_setEntitySlots.erase(*it);
                    it = receiver.erase(it);
                    updateInfo.removed++;
                }
                else
                {
                    it++;
                }
            }
        }
        

        // then addition
        if(!m_pendingIn_setEntitySlots.empty() )
        {
            receiver.insert(
                receiver.end(),
                m_pendingIn_setEntitySlots.begin(),
                m_pendingIn_setEntitySlots.end()
            );

            updateInfo.added = (unsigned int)m_pendingIn_setEntitySlots.size();
        }

        updateInfo.total = (unsigned int)receiver.size();


        // clear pending data
        m_pendingOut_setEntitySlots.clear();
        m_pendingIn_setEntitySlots.clear();


        return updateInfo;
    }

    inline bool CEntityQueryGuard::testSignature( const CEntitySignature& signature ) const
    {
        return signature.has(m_requireSignature) 
            && (m_rejectSignature & signature).empty();
    }

} // namespace chestnut::ecs::internal
