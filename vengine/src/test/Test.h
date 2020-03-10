#pragma once

namespace test {

    class Test
    {
    protected:
        Test() {}
        virtual ~Test() {}

        virtual void onUpdate(float deltaTime) {}
        virtual void onRender() {}
        virtual void onImGuiRender() {}
    };

}
