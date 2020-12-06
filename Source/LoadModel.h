#pragma once

#include "Model.h"
#include "ModelResource.h"
#include "FbxLoad.h"
#include <map>

class LoadModel
{
    static LoadModel* instance;
    std::map<std::string, std::shared_ptr<ModelResource>> modelResources;

public:
    LoadModel() { }
    void Load(ID3D11Device* device, const char* filename);

    // 
    void Load(ID3D11Device* device, const char* filename, const char* modelTag);
    std::shared_ptr<ModelResource>& GetModelResource(const char* modelTag);

    static void Create()
    {
        if (instance != nullptr) return;
        instance = new LoadModel;
    }
    static LoadModel& GetInstance()
    {
        return *instance;
    }
    void D()
    {
        modelResources.clear();
    }
    static void Destory()
    {
        if (instance != nullptr)
        {
            delete instance;
            instance = nullptr;
        }
    }
};

#define pLoadModel LoadModel::GetInstance()
