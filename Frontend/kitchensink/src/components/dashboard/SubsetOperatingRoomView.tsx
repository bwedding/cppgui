import React, { useState } from "react";
import OperatingRoomContent from "./OperatingRoomContent";
import SubsetConfigureORView from "./SubsetConfigureORView";
import { Button } from "@/components/ui/button";
import { Settings } from "lucide-react";
import {
  DropdownMenu,
  DropdownMenuContent,
  DropdownMenuItem,
  DropdownMenuTrigger,
} from "@/components/ui/dropdown-menu";

const SubsetOperatingRoomView: React.FC = () => {
  const [showConfigureORView, setShowConfigureORView] = useState(false);

  return (
    <div className="h-full flex flex-col z-50">
      <div className="bg-neutral-900 border-b border-neutral-700 p-2 flex justify-between items-center z-50">
        <div className="w-36 h-10 flex items-center">
          <img 
            src="../../dist2/logo.png" 
            alt="Logo" 
            className="w-full h-full object-contain"
          />
        </div>
        <DropdownMenu>
          <DropdownMenuTrigger asChild>
            <Button variant="ghost" size="icon" className="text-white hover:bg-neutral-700 z-50">
              <Settings className="h-5 w-5" />
            </Button>
          </DropdownMenuTrigger>
          <DropdownMenuContent className="bg-neutral-800 text-white border-neutral-700 z-50">
            <DropdownMenuItem 
              className="focus:bg-neutral-600 focus:text-white z-50"
              onClick={() => setShowConfigureORView(true)}
            >
              <Settings className="mr-2 h-4 w-4 z-50" />
              Configure View
            </DropdownMenuItem>
          </DropdownMenuContent>
        </DropdownMenu>
      </div>
      <div className="flex-1 mt-10">
        <OperatingRoomContent isSubset={true} />
      </div>
      <SubsetConfigureORView
        open={showConfigureORView}
        onOpenChange={setShowConfigureORView}
      />
    </div>
  );
};

export default SubsetOperatingRoomView;