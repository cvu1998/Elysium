#pragma once

#include <Elysium.h>

class Ground : public Elysium::StaticObject
{
private:

public:
    Ground(const glm::vec2& initialPosition, const glm::vec2& size);

    virtual void Draw() override;
};

