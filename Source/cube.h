#pragma once

#include "obj3d.h"

class Cube
{
private:
	std::unique_ptr<Obj3D> cube;

public:
    Cube();
    ~Cube() {}

    const VECTOR3 GetScale() const { return cube->GetScale(); }
    const VECTOR3 GetRotation() const { return cube->GetRotation(); }
    const VECTOR3 GetPosition() const { return cube->GetPosition(); }

    void SetScale(const VECTOR3& scale) { cube->SetScale(scale); };
    void SetRotation(const VECTOR3& rotation) { cube->SetRotation(rotation); }
    void SetPosition(const VECTOR3& position) { cube->SetPosition(position); }
};