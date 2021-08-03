namespace chestnut
{
    template<class C>
    CComponentStorage<C>::CComponentStorage( segsize segmentSize, segsize initCapacity )
    {
        m_segmentSize = segmentSize;
        m_segmentIDCounter = 0;

        reserve( initCapacity );
    }

    template<class C>
    CComponentStorage<C>::~CComponentStorage() 
    {
        for( auto& [ idx, seg ] : m_mapSegmentIndexToSegment )
        {
            delete seg;
        }
    }

    template<class C>
    segid CComponentStorage<C>::createNewSegment() 
    {
        SegType *segment = new SegType( m_segmentSize );

        m_mapSegmentIndexToSegment[ ++m_segmentIDCounter ] = segment;
        
        return m_segmentIDCounter;
    }

    template<class C>
    segsize CComponentStorage<C>::getSegmentSize() const 
    {
        return m_segmentSize;
    }

    template<class C>
    bool CComponentStorage<C>::hasComponent( entityid entityID ) const 
    {
        for( const auto& [ idx, segment ] : m_mapSegmentIndexToSegment )
        {
            if( segment->hasSlottedComponent( entityID ) )
            {
                return true;
            }
        }

        return false;
    }

    template<class C>
    CComponent *CComponentStorage<C>::storeComponent( entityid entityID ) 
    {
        if( hasComponent( entityID ) )
        {
            return getComponent( entityID );
        }
        
        // get the index of an available segment
        // if there are none, create a new one and push to front of deque of segment indices
        // if there are available ones already, take the ones from the front
        // segments at the front of this deque are of the highest priority to fill
        // as they have the least empty slots left

        segid segIdx;
        if( m_dequeAvailableSegmentIndices.empty() )
        {
            segIdx = createNewSegment();
            m_dequeAvailableSegmentIndices.push_front( segIdx );
        }
        else
        {
            segIdx = m_dequeAvailableSegmentIndices.front();
        }
        
        SegType *segment = m_mapSegmentIndexToSegment[ segIdx ];

        // the segment was in the deque, so we can be sure it has at least one free slot
        CComponent *comp = segment->tryTakeUpSlot( entityID );

        // if segment is full, toss it out of available segments deque
        // we took the segment from the front, so we're going to pop the front
        if( segment->isFull() )
        {
            m_dequeAvailableSegmentIndices.pop_front();
        }

        return comp;
    }

    template<class C>
    CComponent *CComponentStorage<C>::getComponent( entityid entityID ) const
    {
        CComponent *comp = nullptr;
        for( const auto& [ idx, segment ] : m_mapSegmentIndexToSegment )
        {
            comp = segment->getSlottedComponent( entityID );

            if( comp )
            {
                break;
            }
        }

        return comp;
    }

    template<class C>
    void CComponentStorage<C>::eraseComponent( entityid entityID )
    {
        for( const auto& [ idx, segment ] : m_mapSegmentIndexToSegment )
        {
            if( segment->hasSlottedComponent( entityID ) )
            {
                // if the segment is full now, after we free one slot it should be available to be reslotted later
                bool shouldAddToAvailableSegments = segment->isFull();

                segment->freeSlot( entityID );

                if( shouldAddToAvailableSegments )
                {
                    // push front as it's got only 1 slot left at this moment
                    m_dequeAvailableSegmentIndices.push_front( idx );
                }
            }
        }
    }

    template<class C>
    void CComponentStorage<C>::clearComponents() 
    {
        m_dequeAvailableSegmentIndices.clear();

        for( auto& [ idx, segment ] : m_mapSegmentIndexToSegment )
        {
            segment->clearSlots();
            m_dequeAvailableSegmentIndices.push_back( idx );
        }
    }

    template<class C>
    void CComponentStorage<C>::reserve( segsize totalSize ) 
    {
        segsize capacity = getCapacity();
        if( totalSize > capacity  )
        {
            reserveAdditional( totalSize - capacity );
        }
    }

    template<class C>
    void CComponentStorage<C>::reserveAdditional( segsize additionalSize ) 
    {
        segid newSegIdx;
        if( additionalSize < m_segmentSize )
        {
            newSegIdx = createNewSegment();
            m_dequeAvailableSegmentIndices.push_back( newSegIdx );
        }
        else
        {
            segsize div, mod;
            div = additionalSize / m_segmentSize;
            mod = additionalSize % m_segmentSize;

            segsize segmentsToCreate;
            if( mod > 0 )
            {
                segmentsToCreate = div + 1;
            }
            else
            {
                segmentsToCreate = div;
            }
            

            for (segsize i = 0; i < segmentsToCreate; i++)
            {
                newSegIdx = createNewSegment();
                m_dequeAvailableSegmentIndices.push_back( newSegIdx );
            }
        }
    }

    template<class C>
    void CComponentStorage<C>::resize( segsize targetSize ) 
    {
        segsize capacity = getCapacity();

        if( targetSize > capacity )
        {
            // if the target size is greater than current capacity just allocate simply memory
            reserveAdditional( targetSize - capacity );
        }
        else if( targetSize < capacity )
        {
            // if the target size is lesser than current capacity we attempt to deallocate 
            // as many empty segments as we can if there are any

            for( auto it = m_dequeAvailableSegmentIndices.begin(); it != m_dequeAvailableSegmentIndices.end(); /*NOP*/ )
            {
                segid idx = *it;

                SegType *segment = m_mapSegmentIndexToSegment[ idx ];

                if( segment->isEmpty() )
                {
                    segsize size = segment->getSize();

                    if( capacity - size >= targetSize )
                    {
                        delete segment;
                        m_mapSegmentIndexToSegment.erase( idx );

                        it = m_dequeAvailableSegmentIndices.erase( it );

                        capacity -= size;
                    }
                    else
                    {
                        // we won't be able to free these segments without going under targetSize, so just stop the loop already
                        break;
                    }
                }
                else
                {
                    ++it;
                }
            }
        }
    }

    template<class C>
    segsize CComponentStorage<C>::getSize() const 
    {
        segsize size = 0;
        for( const auto& [ idx, segment ] : m_mapSegmentIndexToSegment )
        {
            size += segment->getTakenSlotCount();
        }

        return size;
    }

    template<class C>
    segsize CComponentStorage<C>::getCapacity() const 
    {
        segsize capacity = 0;
        for( const auto& [ idx, segment ] : m_mapSegmentIndexToSegment )
        {
            capacity += segment->getSize();
        }

        return capacity;
    }

    template<class C>
    segsize CComponentStorage<C>::getEmptySegmentTotalSize() const 
    {
        segsize size = 0;

        for( const auto& [ idx, segment ] : m_mapSegmentIndexToSegment )
        {
            if( segment->isEmpty() )
            {
                size += segment->getSize();
            }
        }

        return size;
    }

    template<class C>
    void CComponentStorage<C>::removeEmptySegments() 
    {
        auto it = m_dequeAvailableSegmentIndices.begin();
        while( it != m_dequeAvailableSegmentIndices.end() )
        {
            segid idx = *it;

            SegType *segment = m_mapSegmentIndexToSegment[ idx ];

            if( segment->isEmpty() )
            {
                delete segment;
                m_mapSegmentIndexToSegment.erase( idx );

                it = m_dequeAvailableSegmentIndices.erase( it );
            }
            else
            {
                ++it;
            }
        }
    }

} // namespace chestnut
