
#include "LoadModel.h"

LoadModel* LoadModel::instance = nullptr;

void LoadModel::Load(ID3D11Device* device, const char* filename)
{
    std::shared_ptr<ModelResource> resource;

    auto it = modelResources.find(filename);
    if (it != modelResources.end())
    {
        resource = it->second;
    }
    else
    {

        resource = std::make_shared<ModelResource>(device, filename);
        modelResources.insert(std::make_pair(filename, resource));
    }
}

void LoadModel::Load(ID3D11Device* device, const char* filename, const char* modelTag)
{

    std::shared_ptr<ModelResource> resource = nullptr;

    auto it = modelResources.find(modelTag);
    if (it != modelResources.end())
    {
        return;
    }
    else
    {
        resource = std::make_shared<ModelResource>(device, filename);
        modelResources.insert(std::make_pair(modelTag, resource));
        return;
    }
}

std::shared_ptr<ModelResource>& LoadModel::GetModelResource(const char* modelTag)
{
    auto it = modelResources.find(modelTag);
    return it->second;

}


