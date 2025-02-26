import React, { useState } from 'react';
import { Card, CardHeader, CardTitle, CardContent } from '@/components/ui/card';
import { Button } from '@/components/ui/button';
import { FileText, AlertCircle } from 'lucide-react';
import { Alert, AlertDescription } from '@/components/ui/alert';

const FileDialogComponent = () => {
  const [fileData, setFileData] = useState({ 
    filePath: '', 
    content: '', 
    type: '', 
    size: 0,
    preview: '' 
  });
  const [isLoading, setIsLoading] = useState(false);

  const formatFileSize = (bytes : number) => {
    if (bytes < 1024) return bytes + ' bytes';
    if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(2) + ' KB';
    return (bytes / (1024 * 1024)).toFixed(2) + ' MB';
  };

  const openFileDialog = async () => {
    try {
      setIsLoading(true);
      const result = window.chrome.webview.hostObjects.sync.native.FileOpenDialog;
      console.log('Raw result:', result);
      
      // Parse the JSON response
      const fileInfo = JSON.parse(result);
      console.log('Parsed file info:', fileInfo);
      
      setFileData(fileInfo);
    } catch (error) {
      console.error('Error opening file:', error);
      setFileData({ 
        filePath: 'Error opening file', 
        content: '', 
        type: 'error',
        size: 0,
        preview: '' 
      });
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <Card className="w-full max-w-2xl">
      <CardHeader>
        <CardTitle className="flex items-center gap-2">
          <FileText className="h-6 w-6" />
          File Dialog Demo
        </CardTitle>
      </CardHeader>
      <CardContent>
        <div className="space-y-4">
          <Button 
            onClick={openFileDialog}
            className="w-full sm:w-auto"
            disabled={isLoading}
          >
            {isLoading ? 'Opening...' : 'Open File'}
          </Button>

          {fileData.filePath && (
            <div className="space-y-2">
              <p className="text-sm font-medium">Selected File:</p>
              <p className="text-sm text-gray-500 break-all">{fileData.filePath}</p>
              <p className="text-sm text-gray-500">
                Size: {formatFileSize(fileData.size)}
              </p>
            </div>
          )}

          {fileData.type === 'binary' && (
            <Alert>
              <AlertCircle className="h-4 w-4" />
              <AlertDescription>
                {fileData.preview}
              </AlertDescription>
            </Alert>
          )}

          {fileData.type === 'text' && fileData.content && (
            <div className="space-y-2">
              <p className="text-sm font-medium">File Content:</p>
              <div className="bg-gray-50 p-4 rounded-md overflow-auto max-h-96">
                <pre className="text-sm whitespace-pre-wrap break-words">
                  {fileData.content}
                </pre>
              </div>
            </div>
          )}
        </div>
      </CardContent>
    </Card>
  );
};

export default FileDialogComponent;