
#include "cube.h"
#include "load_model.h"

Cube::Cube()
{
	cube = std::make_unique<Obj3D>();
	cube->SetModelResource(pLoadModel.GetModelResource("Cube"));
}
