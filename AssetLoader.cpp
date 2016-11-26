#include "AssetLoader.h"
#include "AssetManager.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

void LoadPrefabs(CAssetManager* assManager, string folder)
{
	vector<string> names;
	WIN32_FIND_DATA fd;

	char search_path[200];
	sprintf(search_path, "%s*.*", folder.c_str());
	HANDLE hFind = ::FindFirstFile(search_path, &fd);
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);

	}
	for (size_t i = 0; i < names.size(); i++)
	{
		string path = folder + names[i];
		assManager->LoadPrefab(path);
	}
}
void LoadModels(CAssetManager* assManager, string folder)
{
	vector<string> names;
	WIN32_FIND_DATA fd;

	char search_path[200];
	sprintf(search_path, "%s*.*", folder.c_str());
	HANDLE hFind = ::FindFirstFile(search_path, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);

	}
	for (size_t i = 0; i < names.size(); i++)
	{
		string path = folder + names[i];
		assManager->LoadModel(path,0);
	}
}