#include "entity_query_supplier.hpp"

#include <exception>

namespace chestnut::ecs::internal
{    
    inline CEntityQuerySupplier::CEntityQuerySupplier(const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature)
    : m_requireSignature(requireSignature),
      m_rejectSignature(rejectSignature)
    {
        if (!(requireSignature & rejectSignature).empty())
        {
            throw std::runtime_error("'Require' and 'reject' signatures can't have any common types");
        }
    }

    inline const CEntitySignature &CEntityQuerySupplier::requireSignature() const
    {
        return m_requireSignature;
    }

    inline const CEntitySignature &CEntityQuerySupplier::rejectSignature() const
    {
        return m_rejectSignature;
    }

    inline const std::unordered_set<entityslot_t> &CEntityQuerySupplier::pendingIn() const
    {
        return m_pendingInEntitySlots;
    }

    inline const std::unordered_set<entityslot_t> &CEntityQuerySupplier::pendingOut() const
    {
        return m_pendingOutEntitySlots;
    }

    inline const std::vector<entityslot_t> &CEntityQuerySupplier::current() const
    {
        return m_currentEntitySlots.dense();
    }

    inline bool CEntityQuerySupplier::proposeEntity(entityslot_t entitySlot, tl::optional<CEntitySignature> prevSign, tl::optional<CEntitySignature> currSign)
    {
        bool prevValid = prevSign.map([&](const auto& sign) { return this->testSignature(sign); }).value_or(false);
        bool currValid = currSign.map([&](const auto& sign) { return this->testSignature(sign); }).value_or(false);

        if (!prevValid && currValid)
        {
            this->m_pendingOutEntitySlots.erase(entitySlot);

            if (!this->m_currentEntitySlots.contains(entitySlot))
            {
                this->m_pendingInEntitySlots.insert(entitySlot);
            }

            return true;
        }
        else if (prevValid && !currValid)
        {
            this->m_pendingInEntitySlots.erase(entitySlot);

            if (this->m_currentEntitySlots.contains(entitySlot))
            {
                this->m_pendingOutEntitySlots.insert(entitySlot);
            }
            
            return true;
        }

        return false;
    }

    inline bool CEntityQuerySupplier::hasPendingEntities() const
    {
        return !m_pendingInEntitySlots.empty() || !m_pendingOutEntitySlots.empty();
    }

    inline void CEntityQuerySupplier::processPendingEntities()
    {
        if(!m_pendingOutEntitySlots.empty())
        {
            for (auto slot : m_pendingOutEntitySlots)
            {
                m_currentEntitySlots.erase(slot);
            }
        }
        
        if(!m_pendingInEntitySlots.empty() )
        {
            for (auto slot : m_pendingInEntitySlots)
            {
                m_currentEntitySlots.insert(slot, slot);
            }
        }

        // clear pending data
        m_pendingOutEntitySlots.clear();
        m_pendingInEntitySlots.clear();
    }

    inline bool CEntityQuerySupplier::testSignature(const CEntitySignature& signature) const
    {
        return signature.has(m_requireSignature) 
            && (m_rejectSignature & signature).empty();
    }

} // namespace chestnut::ecs::internal
