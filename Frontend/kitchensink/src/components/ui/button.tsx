import * as React from "react"
import { Slot } from "@radix-ui/react-slot"
import { cva, type VariantProps } from "class-variance-authority"
import { useEventStore } from "@/lib/EventStore"
import { cn } from "@/lib/utils"

const buttonVariants = cva(
  "inline-flex items-center justify-center whitespace-nowrap rounded-md text-sm font-medium transition-colors focus-visible:outline-none focus-visible:ring-1 focus-visible:ring-ring disabled:pointer-events-none disabled:opacity-50",
  {
    variants: {
      variant: {
        default:
          "bg-primary text-primary-foreground shadow hover:bg-primary/90",
        destructive:
          "bg-destructive text-destructive-foreground shadow-sm hover:bg-destructive/90",
        outline:
          "border border-input bg-background shadow-sm hover:bg-accent hover:text-accent-foreground",
        secondary:
          "bg-secondary text-secondary-foreground shadow-sm hover:bg-secondary/80",
        ghost: "hover:bg-accent hover:text-accent-foreground",
        link: "text-primary underline-offset-4 hover:underline",
      },
      size: {
        default: "h-9 px-4 py-2",
        sm: "h-8 rounded-md px-3 text-xs",
        lg: "h-10 rounded-md px-8",
        icon: "h-9 w-9",
      },
    },
    defaultVariants: {
      variant: "default",
      size: "default",
    },
  }
)

export interface ButtonProps
  extends React.ButtonHTMLAttributes<HTMLButtonElement>,
    VariantProps<typeof buttonVariants> {
  asChild?: boolean
  /** ID to be sent with the button click event */
  buttonId?: string
  /** Additional metadata to be sent with the event */
  eventMetadata?: Record<string, any>
  /** Whether to disable event dispatch */
  noEvent?: boolean
}

const Button = React.forwardRef<HTMLButtonElement, ButtonProps>(
  ({ 
    className, 
    variant, 
    size, 
    asChild = false, 
    buttonId,
    eventMetadata = {},
    noEvent = false,
    onClick,
    ...props 
  }, ref) => {
    const sendEvent = useEventStore(state => state.sendEvent)
    const Comp = asChild ? Slot : "button"

    const handleClick = React.useCallback((e: React.MouseEvent<HTMLButtonElement>) => {
      // Call the original onClick handler if provided
      onClick?.(e)

      // Don't dispatch event if noEvent is true or button is disabled
      if (noEvent || props.disabled) return

      // Dispatch the button click event
      sendEvent({
        type: 'BUTTON_CLICK',
        buttonId: buttonId || props.id || props.name || 'unnamed-button',
        value: true,
        source: 'user-interface',
        timestamp: Date.now(),
        action: eventMetadata.action || '',
        control: eventMetadata.control || '',
        parameter: eventMetadata.parameter || '',
        currentState: eventMetadata.currentState || 0,
        metadata: {
          ...eventMetadata,
          variant,
          size,
        }
      })
    }, [sendEvent, onClick, noEvent, props.disabled, buttonId, props.id, props.name, variant, size, eventMetadata])

    return (
      <Comp
        className={cn(buttonVariants({ variant, size, className }))}
        ref={ref}
        onClick={handleClick}
        {...props}
      />
    )
  }
)
Button.displayName = "Button"

export { Button, buttonVariants }

// Example usage:
/*
// Basic usage - will send event with default ID
<Button>Click Me</Button>

// With custom button ID
<Button buttonId="start-process">Start</Button>

// With additional metadata
<Button 
  buttonId="settings-save"
  eventMetadata={{
    section: 'user-preferences',
    changes: ['theme', 'language']
  }}
>
  Save Settings
</Button>

// Disable event dispatch
<Button noEvent>Don't Send Event</Button>
*/