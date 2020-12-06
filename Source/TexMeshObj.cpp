#include "TexMeshObj.h"

void TexMeshObj::Create(ID3D11Device* device, const XMFLOAT3& scale, const XMFLOAT3& angle, const XMFLOAT3& position, const ObjectType& type, const wchar_t* filename)
{
    this->scale = scale;
    this->angle = angle;
    this->position = position;
    this->type = type;

    switch (type)
    {
    case ObjectType::GeometricBorad:
        //obj = std::make_shared<TexBoard>(device, filename);
        //obj = std::make_shared<TexPlain>(device, filename);
        break;

    case ObjectType::GeometricCube:
        obj = std::make_shared<TexCube>(device, filename);
        break;

    case ObjectType::GeometricSphere:
        obj = std::make_shared<TexSphere>(device, filename);
        break;

    case ObjectType::MAX:
    default:

        assert(!"");
        break;
    }
}

void TexMeshObj::Create(ID3D11Device* device, const XMFLOAT3& scale, const XMFLOAT3& angle, const XMFLOAT3& position, const ObjectType& type, std::shared_ptr<Texture>& texture)
{
    this->scale = scale;
    this->angle = angle;
    this->position = position;
    this->type = type;

    switch (type)
    {
    case ObjectType::GeometricBorad:
        obj = std::make_shared<TexBoard>(device, texture);
        break;

    case ObjectType::GeometricCube:
        obj = std::make_shared<TexCube>(device, texture);
        break;

    case ObjectType::GeometricSphere:
        obj = std::make_shared<TexSphere>(device, texture);
        break;

    case ObjectType::MAX:
    default:

        assert(!"");
        break;
    }
}

int TexMeshObj::RayPick(const XMFLOAT3& startPosition, const XMFLOAT3& endPosition, XMFLOAT3* outPosition, XMFLOAT3* outNormal)
{
    DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&world);
    DirectX::XMMATRIX inverseTransform = DirectX::XMMatrixInverse(nullptr, worldTransform);

    // オブジェクト空間でのレイに変換
    DirectX::XMVECTOR worldStart = DirectX::XMLoadFloat3(&startPosition);
    DirectX::XMVECTOR worldEnd = DirectX::XMLoadFloat3(&endPosition);
    DirectX::XMVECTOR localStart = DirectX::XMVector3TransformCoord(worldStart, inverseTransform);
    DirectX::XMVECTOR localEnd = DirectX::XMVector3TransformCoord(worldEnd, inverseTransform);

    // レイピック
    float outDistance;
    XMFLOAT3 start, end;
    DirectX::XMStoreFloat3(&start, localStart);
    DirectX::XMStoreFloat3(&end, localEnd);
    int ret = obj->RayPick(start, end, outPosition, outNormal, &outDistance);
    if (ret != -1)
    {
        // オブジェクト空間からワールド空間へ変換
        DirectX::XMVECTOR localPosition = DirectX::XMLoadFloat3(outPosition);
        DirectX::XMVECTOR localNormal = DirectX::XMLoadFloat3(outNormal);
        DirectX::XMVECTOR worldPosition = DirectX::XMVector3TransformCoord(localPosition, worldTransform);
        DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(localNormal, worldTransform);

        DirectX::XMStoreFloat3(outPosition, worldPosition);
        DirectX::XMStoreFloat3(outNormal, worldNormal);
    }

    return ret;
}

void TexMeshObj::SetTexture(ID3D11Device* device, const wchar_t* filename)
{
    texture = std::make_shared<Texture>();
    texture->Load(device, filename);
    obj->SetTexture(texture);
}

void TexMeshObj::SetGeometric(ID3D11Device* device, const ObjectType& type)
{
    this->type = type;
    switch (type)
    {
    case ObjectType::GeometricBorad:
        obj = std::make_shared<TexBoard>(device, texture);
        break;

    case ObjectType::GeometricCube:
        obj = std::make_shared<TexCube>(device, texture);
        break;

    case ObjectType::GeometricSphere:
        obj = std::make_shared<TexSphere>(device, texture);
        break;

    case ObjectType::MAX:
    default:

        assert(!"");
        break;
    }
}



