# PowerShell Script

# Find and delete all "node_modules" directories
Get-ChildItem -Path ".." -Recurse -Directory -Filter "node_modules" | ForEach-Object {
    Write-Host "Deleting $($_.FullName)..."
    Remove-Item -Recurse -Force -Path $_.FullName
}

# Find and delete all "dist" directories
Get-ChildItem -Path ".." -Recurse -Directory -Filter "dist" | ForEach-Object {
    Write-Host "Deleting $($_.FullName)..."
    Remove-Item -Recurse -Force -Path $_.FullName
}

# Find and delete all ".nx" directories
Get-ChildItem -Path ".." -Recurse -Directory -Filter ".nx" | ForEach-Object {
    Write-Host "Deleting $($_.FullName)..."
    Remove-Item -Recurse -Force -Path $_.FullName
}

Write-Host "Finished"
