namespace chestnut::ecs::internal
{    
    inline CEntityQueryGuard::CEntityQueryGuard(CComponentStorage *componentStorage, const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature)
    : m_targetQuery(componentStorage, requireSignature, rejectSignature)
    {

    }

    inline void CEntityQueryGuard::enqueueEntity( entityid_t entityID ) 
    {
        m_pendingOut_setEntityIDs.erase(entityID);
        m_pendingIn_setEntityIDs.insert(entityID);
    }

    inline void CEntityQueryGuard::dequeueEntity( entityid_t entityID ) 
    {
        m_pendingIn_setEntityIDs.erase(entityID);
        m_pendingOut_setEntityIDs.insert(entityID);
    }

    inline SEntityQueryUpdateInfo CEntityQueryGuard::updateQuery() 
    {
        SEntityQueryUpdateInfo updateInfo {0, 0, 0};

        // first do the removal
        if(!m_pendingOut_setEntityIDs.empty())
        {
            for(auto it = m_targetQuery.m_vecEntityIDs.begin(); it != m_targetQuery.m_vecEntityIDs.end(); /*NOP*/)
            {
                if(m_pendingOut_setEntityIDs.find(*it) != m_pendingOut_setEntityIDs.end())
                {
                    m_pendingOut_setEntityIDs.erase(*it);
                    it = m_targetQuery.m_vecEntityIDs.erase(it);
                    updateInfo.removed++;
                }
                else
                {
                    it++;
                }
            }
        }
        

        // then addition
        if(!m_pendingIn_setEntityIDs.empty() )
        {
            m_targetQuery.m_vecEntityIDs.insert(
                m_targetQuery.m_vecEntityIDs.end(),
                m_pendingIn_setEntityIDs.begin(),
                m_pendingIn_setEntityIDs.end()
            );

            updateInfo.added = (unsigned int)m_pendingIn_setEntityIDs.size();
        }

        updateInfo.total = (unsigned int)m_targetQuery.m_vecEntityIDs.size();


        // clear pending data
        m_pendingOut_setEntityIDs.clear();
        m_pendingIn_setEntityIDs.clear();


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
