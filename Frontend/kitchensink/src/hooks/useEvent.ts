import { useCallback } from 'react';
import { useEventStore } from '../lib/EventStore';
import { ApplicationEvent, EventType } from '../types/types';

export const useEvent = () => {
    const sendEvent = useEventStore(state => state.sendEvent);
    
    const dispatch = useCallback((
      type: EventType,
      data: Omit<ApplicationEvent, 'type' | 'timestamp'>
    ) => {
      sendEvent({
        ...data,
        type,
        timestamp: Date.now()
      } as ApplicationEvent);
    }, [sendEvent]);
  
    return dispatch;
  };
