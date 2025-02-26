import * as React from "react"
import { useEventStore } from "@/lib/eventstore" // Adjust path as needed
import { cn } from "@/lib/utils"

export interface InputProps
  extends React.InputHTMLAttributes<HTMLInputElement> {
  /** ID to be sent with the input event */
  inputId?: string
  /** Additional metadata to be sent with the event */
  eventMetadata?: Record<string, any>
  /** Whether to send events to the backend */
  sendToBackend?: boolean
  /** When to send events: 'onChange' | 'onBlur' | 'both' */
  sendEventOn?: 'onChange' | 'onBlur' | 'both'
  /** Debounce time in ms for onChange events (0 to disable) */
  debounceMs?: number
}

const Input = React.forwardRef<HTMLInputElement, InputProps>(
  ({ 
    className, 
    type,
    inputId,
    eventMetadata = {},
    sendToBackend = false,
    sendEventOn = 'both',
    debounceMs = 300,
    onChange,
    onBlur,
    ...props 
  }, ref) => {
    const sendEvent = useEventStore(state => state.sendEvent)
    const [localValue, setLocalValue] = React.useState(props.value || props.defaultValue || '')
    const debounceTimer = React.useRef<NodeJS.Timeout>()

    const sendInputEvent = React.useCallback((
      value: string | number | readonly string[],
      triggerType: 'change' | 'blur'
    ) => {
      if (!sendToBackend) return
      if (triggerType === 'change' && sendEventOn === 'onBlur') return
      if (triggerType === 'blur' && sendEventOn === 'onChange') return

      sendEvent({
        type: 'VALUE_CHANGE',
        fieldId: inputId || props.id || props.name || 'unnamed-input',
        value,
        previousValue: localValue,
        source: 'input-field',
        timestamp: Date.now(),
        inputType: type,
        triggerType,
        ...eventMetadata
      })
    }, [sendToBackend, sendEventOn, inputId, props.id, props.name, type, eventMetadata, localValue, sendEvent])

    const handleChange = React.useCallback((e: React.ChangeEvent<HTMLInputElement>) => {
      const newValue = e.target.value
      setLocalValue(newValue)
      
      // Call original onChange handler
      onChange?.(e)

      // Handle debounced event dispatch
      if (debounceMs > 0) {
        if (debounceTimer.current) {
          clearTimeout(debounceTimer.current)
        }
        debounceTimer.current = setTimeout(() => {
          sendInputEvent(newValue, 'change')
        }, debounceMs)
      } else {
        sendInputEvent(newValue, 'change')
      }
    }, [onChange, sendInputEvent, debounceMs])

    const handleBlur = React.useCallback((e: React.FocusEvent<HTMLInputElement>) => {
      // Call original onBlur handler
      onBlur?.(e)

      // Clear any pending debounced onChange events
      if (debounceTimer.current) {
        clearTimeout(debounceTimer.current)
      }

      sendInputEvent(e.target.value, 'blur')
    }, [onBlur, sendInputEvent])

    // Cleanup debounce timer
    React.useEffect(() => {
      return () => {
        if (debounceTimer.current) {
          clearTimeout(debounceTimer.current)
        }
      }
    }, [])

    return (
      <input
        type={type}
        className={cn(
          "flex h-9 w-full rounded-md border border-input bg-transparent px-3 py-1 text-sm shadow-sm transition-colors file:border-0 file:bg-transparent file:text-sm file:font-medium placeholder:text-muted-foreground focus-visible:outline-none focus-visible:ring-1 focus-visible:ring-ring disabled:cursor-not-allowed disabled:opacity-50",
          className
        )}
        ref={ref}
        onChange={handleChange}
        onBlur={handleBlur}
        {...props}
      />
    )
  }
)
Input.displayName = "Input"

export { Input }

// Example usage:
/*
// Basic input without backend events
<Input placeholder="Local only" />

// Input that sends events to backend
<Input 
  inputId="user-name"
  sendToBackend={true}
  placeholder="Enter name"
/>

// Input that only sends events on blur
<Input 
  inputId="settings-value"
  sendToBackend={true}
  sendEventOn="onBlur"
  eventMetadata={{
    section: 'user-preferences',
    setting: 'theme'
  }}
/>

// Input with custom debounce time
<Input 
  inputId="search"
  sendToBackend={true}
  debounceMs={500}
  placeholder="Search..."
/>
*/