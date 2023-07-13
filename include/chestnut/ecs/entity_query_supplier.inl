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

    inline bool CEntityQuerySupplier::proposeEntity(entityslot_t entitySlot, tl::optional<CEntitySignature> prevSign, tl::optional<CEntitySignature> currSign)
    {
        bool prevValid = prevSign.map([&](const auto& sign) { return this->testSignature(sign); }).value_or(false);
        bool currValid = currSign.map([&](const auto& sign) { return this->testSignature(sign); }).value_or(false);
        bool isInCurrent = this->m_currentEntitySlots.find(entitySlot) != this->m_currentEntitySlots.end();

        if (!prevValid && currValid)
        {
            this->m_pendingOutEntitySlots.erase(entitySlot);

            if (!isInCurrent)
            {
                this->m_pendingInEntitySlots.insert(entitySlot);
            }

            return true;
        }
        else if (prevValid && !currValid)
        {
            this->m_pendingInEntitySlots.erase(entitySlot);

            if (isInCurrent)
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

    inline void CEntityQuerySupplier::processPendingEntities(
        std::vector<entityslot_t>& destCurrent, 
        std::vector<entityslot_t>& destAcquired, 
        std::vector<entityslot_t>& destDiscarded)
    {
        destAcquired.clear();
        std::copy(m_pendingInEntitySlots.begin(), m_pendingInEntitySlots.end(), std::back_inserter(destAcquired));

        destDiscarded.clear();
        std::copy(m_pendingOutEntitySlots.begin(), m_pendingOutEntitySlots.end(), std::back_inserter(destDiscarded));

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
                m_currentEntitySlots.insert(slot);
            }
        }

        destCurrent.clear();
        std::copy(m_currentEntitySlots.begin(), m_currentEntitySlots.end(), std::back_inserter(destCurrent));

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
