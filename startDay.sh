# start-of-day.sh
#!/bin/bash
echo "Getting latest code..."
git pull

echo -e "\nUpdating frontend..."
cd frontend
git checkout master  # or main, depending on your branch name
git pull
cd ..

echo -e "\nUpdating backend..."
cd backend
git checkout master
git pull
cd ..

git submodule update --remote --merge

echo -e "\nFrontend status:"
cd frontend
git status

echo -e "\nBackend status:"
cd ../backend
git status

echo -e "\nReady to start working!"