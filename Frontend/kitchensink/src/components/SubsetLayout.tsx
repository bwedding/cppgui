import React from 'react';
import { Card } from '@/components/ui/card.tsx';

interface SubsetLayoutProps {
  children: React.ReactNode;
}

const SubsetLayout: React.FC<SubsetLayoutProps> = ({ children }) => {
  return (
    <div className="min-h-screen bg-background">
      <main className="flex min-h-screen flex-col">
        <div className="flex-1 space-y-4 p-4">
          <Card className="p-4">
            {children}
          </Card>
        </div>
      </main>
    </div>
  );
};

export default SubsetLayout;
