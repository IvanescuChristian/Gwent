#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        struct Card
        {
            glm::vec2 position;
            glm::vec2 targetPosition;
            int type;
            int value;
            float scale;
            float angle;
        };

        Tema1();
        ~Tema1();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void RenderBoard();
        void RenderCard(const Card &card);
        void UpdateCard(Card &card, float deltaTimeSeconds);
        glm::vec2 ScreenToWorld(int mouseX, int mouseY);

     protected:
        glm::mat3 modelMatrix;

        float logicWidth, logicHeight;
        float cardWidth, cardHeight;
        float boardLeft, boardRight;
        float boardBottom, boardMiddle, boardTop;
        float playerHandY, enemyHandY;
        float speed;

        Card card;
    };
}   // namespace m1
