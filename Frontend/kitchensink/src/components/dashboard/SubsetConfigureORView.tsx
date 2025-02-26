import React from "react";
import { useAtom } from "jotai";
import { Checkbox } from "@/components/ui/checkbox";
import {
  Dialog,
  DialogContent,
  DialogHeader,
  DialogTitle,
} from "@/components/ui/dialog";
import { pressureCardVisibilityAtom } from "@/lib/subset-datastore";

interface SubsetConfigureORViewProps {
  open: boolean;
  onOpenChange: (open: boolean) => void;
}

const SubsetConfigureORView: React.FC<SubsetConfigureORViewProps> = ({
  open,
  onOpenChange,
}) => {
  const [visibility, setVisibility] = useAtom(pressureCardVisibilityAtom);

  const handleCheckboxChange = (key: string) => {
    setVisibility((prev) => ({
      ...prev,
      [key]: !prev[key as keyof typeof prev],
    }));
  };

  return (
    <Dialog open={open} onOpenChange={onOpenChange}>
      <DialogContent className="bg-neutral-900 text-white border-neutral-700">
        <DialogHeader>
          <DialogTitle>Configure OR View</DialogTitle>
        </DialogHeader>
        <div className="space-y-4">
          {[
            { id: "MAP", label: "MAP Pressure Card" },
            { id: "PAP", label: "PAP Pressure Card" },
            { id: "CVP", label: "CVP Pressure Card" },
            { id: "AOP", label: "AOP Pressure Card" },
            { id: "IVC", label: "IVC Pressure Card" },
            { id: "LAP", label: "LAP Pressure Card" },
            { id: "RAP", label: "RAP Pressure Card" },
            { id: "LCO", label: "LCO Pressure Card" },
            { id: "RCO", label: "RCO Pressure Card" },
          ].map(({ id, label }) => (
            <div key={id} className="flex items-center space-x-2">
              <Checkbox
                id={id}
                checked={visibility[id as keyof typeof visibility]}
                onCheckedChange={() => handleCheckboxChange(id)}
                className="border-white data-[state=checked]:bg-blue-600 data-[state=checked]:border-blue-600"
              />
              <label
                htmlFor={id}
                className="text-sm font-medium leading-none peer-disabled:cursor-not-allowed peer-disabled:opacity-70"
              >
                {label}
              </label>
            </div>
          ))}
        </div>
      </DialogContent>
    </Dialog>
  );
};

export default SubsetConfigureORView;
