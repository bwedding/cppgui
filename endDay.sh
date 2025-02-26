#!/bin/bash

commit_if_changes() {
    if [[ $(git status --porcelain) ]]; then
        echo "Changes detected in $(pwd)"
        echo "Commit message for these changes:"
        read -e commit_msg
        git add .
        git commit -m "$commit_msg"
        return 0
    else
        echo "No changes to commit in $(pwd)"
        return 1
    fi
}

echo "Checking frontend changes..."
cd frontend
git checkout master  # or main, depending on your branch name
commit_changes_frontend=$?
if commit_if_changes; then
    commit_changes_frontend=1
fi
git push
echo -e "\nChecking backend changes..."
cd ../backend
git checkout master
commit_changes_backend=$?
if commit_if_changes; then
    commit_changes_backend=1
fi
git push

echo -e "\nUpdating parent repo..."
cd ..
git submodule update --remote --merge

# Only add submodules if they had changes
if [ $commit_changes_frontend -eq 1 ]; then
    git add frontend
fi
if [ $commit_changes_backend -eq 1 ]; then
    git add backend
fi

# Check if we have any changes to commit in the parent repo
if [[ $(git status --porcelain) ]]; then
    echo "Commit message for parent repo:"
    read -e parent_commit_msg
    git commit -a -m "$parent_commit_msg"
    git push
    echo -e "\nAll changes committed and pushed!"
else
    echo -e "\nNo changes to commit in parent repo. All pushes complete!"
fi
