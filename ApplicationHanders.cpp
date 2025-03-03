// AUTO-GENERATED - DO NOT MODIFY
#include "InputHandler.h"

class ApplicationHandlers : public InputHandler {
    // Auto-generated overrides
    { { #each inputs } }
    void Handle{ {pascalCase id} }() override {
        // TODO: Implement {{id}} handler
        // Example: GetSystem().PrintService().StartJob();
    }
    {{ / each}}
};