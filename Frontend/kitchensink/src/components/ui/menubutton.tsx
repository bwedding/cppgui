import React from 'react';

interface MenuButtonProps {
  onClick?: () => void;
  children: React.ReactNode;
  disabled?: boolean;
  [key: string]: any; // Allow any other props
}

const MenuButton = ({
  onClick,
  children,
  disabled = false,
  ...props
}: MenuButtonProps) => {
  return (
    <button
      onClick={onClick}
      disabled={disabled}
      className={`p-2 hover:bg-neutral-900 rounded text-gray-400 hover:text-gray-100 cursor-pointer ${disabled ? 'opacity-50 cursor-not-allowed' : ''}`}
      {...props}
    >
      {children}
    </button>
  );
};

export default MenuButton;
