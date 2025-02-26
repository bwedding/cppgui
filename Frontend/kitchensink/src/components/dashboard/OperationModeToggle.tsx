import React from 'react';
import { useEventStore } from '@/lib/EventStore.ts';
import { SwitchToggleEvent } from '@/types/types.ts';

interface OperationModeToggleProps {
  leftLabel?: string;
  rightLabel?: string;
  defaultValue?: boolean;
  onChange?: (isChecked: boolean) => void;
  isChecked: boolean
  switchId?: string
  eventMetadata?: Record<string, any>
  noEvent?: boolean
  onToggle: (checked: boolean) => void
  mode: 'manual' | 'auto'
}

const AUTO_STATE = 'auto';
const MANUAL_STATE = 'manual';

const OperationModeToggle = ({
  isChecked, 
  switchId = 'auto-manual-control',
  eventMetadata = {}, 
  noEvent = false,
  onToggle, 
  leftLabel = "Manual",
  rightLabel = "Auto",
  mode
}: OperationModeToggleProps) => {

  const sendEvent = useEventStore(state => state.sendEvent)

  const handleToggle = React.useCallback(() => {
    onToggle(!isChecked)
    if (noEvent) return

    const event: SwitchToggleEvent = {
      type: 'SWITCH_TOGGLE',
      switchId,
      state: !isChecked ? AUTO_STATE : MANUAL_STATE,
      source: 'user-interface',
      timestamp: Date.now()
    };
    console.log('Sending switch toggle event', event);
    sendEvent(event)
  }, [sendEvent, isChecked, switchId, noEvent, onToggle])

  return (
    <div className="flex items-center space-x-3 mt-4">
      <span className={`text-sm ${isChecked ? 'text-gray-500' : 'text-gray-300'}`}>
        {leftLabel}
      </span>
      
      <button
        role="switch"
        aria-checked={isChecked}
        onClick={handleToggle}
        className={`
          relative 
          w-14 
          h-7 
          rounded-full 
          bg-neutral-950
          border border-neutral-700
          transition-colors 
          duration-200
          focus:outline-none
        `}
      >
        <span
          className={`
            absolute 
            left-0.5 
            top-[12%] 
            w-[20px] 
            h-[20px] 
            rounded-full
            bg-neutral-800 
            bg-gradient-to-b
            shadow-inner
            border border-neutral-700
            transition-transform 
            duration-300 
            transform
            ${isChecked ? 'translate-x-7' : 'translate-x-0'}
          `}
        />
      </button>

      <span className={`text-sm ${isChecked ? 'text-gray-300' : 'text-gray-500'}`}>
        {rightLabel}
      </span>
    </div>
  );
};

export default OperationModeToggle;