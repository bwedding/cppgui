import * as React from "react"
import * as TooltipPrimitive from "@radix-ui/react-tooltip"

import { cn } from "lib/utils"

// Memoize the provider component
const TooltipProvider = React.memo(TooltipPrimitive.Provider)
TooltipProvider.displayName = 'MemoizedTooltipProvider'

// Use the original Tooltip (Root) as it's a context provider
const Tooltip = TooltipPrimitive.Root

// Memoize the trigger component
const TooltipTrigger = React.memo(TooltipPrimitive.Trigger)
TooltipTrigger.displayName = 'MemoizedTooltipTrigger'

// Memoize the content component with forwardRef
const TooltipContent = React.memo(
  React.forwardRef<
    React.ElementRef<typeof TooltipPrimitive.Content>,
    React.ComponentPropsWithoutRef<typeof TooltipPrimitive.Content>
  >(({ className, sideOffset = 4, ...props }, ref) => (
    <TooltipPrimitive.Content
      ref={ref}
      sideOffset={sideOffset}
      className={cn(
        "z-50 overflow-hidden rounded-md bg-primary px-3 py-1.5 text-xs text-primary-foreground animate-in fade-in-0 zoom-in-95 data-[state=closed]:animate-out data-[state=closed]:fade-out-0 data-[state=closed]:zoom-out-95 data-[side=bottom]:slide-in-from-top-2 data-[side=left]:slide-in-from-right-2 data-[side=right]:slide-in-from-left-2 data-[side=top]:slide-in-from-bottom-2",
        className
      )}
      {...props}
    />
  ))
)
TooltipContent.displayName = 'MemoizedTooltipContent'

export { Tooltip, TooltipTrigger, TooltipContent, TooltipProvider }
