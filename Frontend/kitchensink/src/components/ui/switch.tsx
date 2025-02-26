import * as React from "react"
import { useEventStore } from "@/lib/EventStore"
import * as SwitchPrimitives from "@radix-ui/react-switch"
import { cn } from "@/lib/utils"

export interface SwitchProps extends React.ComponentPropsWithoutRef<typeof SwitchPrimitives.Root> {
  /** ID to be sent with the switch event */
  switchId?: string
  /** Additional metadata to be sent with the event */
  eventMetadata?: Record<string, any>
  /** Whether to disable event dispatch */
  noEvent?: boolean
}

const Switch = React.forwardRef<
  React.ElementRef<typeof SwitchPrimitives.Root>,
  SwitchProps
>(({ 
  className, 
  switchId,
  eventMetadata = {},
  noEvent = false,
  onCheckedChange,
  ...props 
}, ref) => {
  const sendEvent = useEventStore(state => state.sendEvent)

  const handleCheckedChange = React.useCallback((checked: boolean) => 
  {
    // Call the original handler if provided
    onCheckedChange?.(checked)

    // Don't dispatch event if noEvent is true or switch is disabled
    if (noEvent || props.disabled) return

    // Dispatch the switch toggle event
    sendEvent({
      type: 'SWITCH_TOGGLE',
      switchId: switchId || props.id || props.name || 'unnamed-switch',
      value: checked,
      source: 'user-interface',
      timestamp: Date.now(),
      metadata: {
        ...eventMetadata,
        disabled: props.disabled
      }
    })
    console.log('Switch toggled', {
      switchId: switchId || props.id || props.name || 'unnamed-switch',
      value: checked,
      source: 'user-interface',
      timestamp: Date.now(),
      metadata: {
        ...eventMetadata,
        disabled: props.disabled
      }
    })
  }, [sendEvent, onCheckedChange, noEvent, props.disabled, switchId, props.id, props.name, eventMetadata])

  return (
    <SwitchPrimitives.Root
      className={cn(
        "peer inline-flex h-5 w-9 shrink-0 cursor-pointer items-center rounded-full border-2 border-transparent shadow-sm transition-colors focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 focus-visible:ring-offset-background disabled:cursor-not-allowed disabled:opacity-50 data-[state=checked]:bg-primary data-[state=unchecked]:bg-input",
        className
      )}
      onCheckedChange={handleCheckedChange}
      {...props}
      ref={ref}
    >
      <SwitchPrimitives.Thumb
        className={cn(
          "pointer-events-none block h-4 w-4 rounded-full bg-background shadow-lg ring-0 transition-transform data-[state=checked]:translate-x-4 data-[state=unchecked]:translate-x-0"
        )}
      />
    </SwitchPrimitives.Root>
  )
})

Switch.displayName = SwitchPrimitives.Root.displayName

export { Switch }