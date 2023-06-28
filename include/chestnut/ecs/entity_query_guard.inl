namespace chestnut::ecs::internal
{    
    inline CEntityQueryGuard::CEntityQueryGuard(CComponentStorage *componentStorage, const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature)
    : m_targetQuery(componentStorage, requireSignature, rejectSignature)
    {

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

    inline SEntityQueryUpdateInfo CEntityQueryGuard::updateQuery() 
    {
        SEntityQueryUpdateInfo updateInfo {0, 0, 0};

        // first do the removal
        if(!m_pendingOut_setEntitySlots.empty())
        {
            for(auto it = m_targetQuery.m_vecEntitySlots.begin(); it != m_targetQuery.m_vecEntitySlots.end(); /*NOP*/)
            {
                if(m_pendingOut_setEntitySlots.find(*it) != m_pendingOut_setEntitySlots.end())
                {
                    m_pendingOut_setEntitySlots.erase(*it);
                    it = m_targetQuery.m_vecEntitySlots.erase(it);
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
            m_targetQuery.m_vecEntitySlots.insert(
                m_targetQuery.m_vecEntitySlots.end(),
                m_pendingIn_setEntitySlots.begin(),
                m_pendingIn_setEntitySlots.end()
            );

            updateInfo.added = (unsigned int)m_pendingIn_setEntitySlots.size();
        }

        updateInfo.total = (unsigned int)m_targetQuery.m_vecEntitySlots.size();


        // clear pending data
        m_pendingOut_setEntitySlots.clear();
        m_pendingIn_setEntitySlots.clear();


        return updateInfo;
    }

    inline bool CEntityQueryGuard::testQuery( const CEntitySignature& signature ) const
    {
        return signature.hasAllFrom(m_targetQuery.m_requireSignature) && !signature.hasAnyFrom(m_targetQuery.m_rejectSignature);
    }

    inline const CEntityQuery& CEntityQueryGuard::getQuery() const
    {
        return m_targetQuery;
    }

    inline CEntityQuery& CEntityQueryGuard::getQuery() 
    {
        return m_targetQuery;
    }

} // namespace chestnut::ecs::internal
