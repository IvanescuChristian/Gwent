#pragma once

#include "components/simple_scene.h"
#include "components/text_renderer.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        struct CardState
        {
            bool isFlipped;
        };

        struct Card
        {
            glm::vec2 position;
            glm::vec2 targetPosition;
            glm::vec2 handPosition;
            int type;
            int value;
            float scale;
            float angle;
            CardState state;
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
        void RenderCard(const Card &card, bool isEnemy);
        void RenderDeck();
        void RenderHearts();
        // Draw the heads-up display
        void DrawHUD();
        void UpdateCard(Card &card, float deltaTimeSeconds);
        glm::vec2 ScreenToWorld(int mouseX, int mouseY);
        bool IsMouseOverCard(const Card &card, glm::vec2 mousePosition);

     protected:
        glm::mat3 modelMatrix;

        float logicWidth, logicHeight;
        float cardWidth, cardHeight;
        float boardLeft, boardRight;
        float boardBottom, boardMiddle, boardTop;
        float playerHandY, enemyHandY;
        float speed;

        static const int kCardsInHand = 9;
        Card playerCards[kCardsInHand];
        Card enemyCards[kCardsInHand];

        float cardGap;
        float deckX, deckY;
        float heartX, playerHeartY, enemyHeartY;

        int draggedCard;
        glm::vec2 dragOffset;
        glm::vec2 pressPosition;
        float clickThreshold;

        // The actual renderer object
        gfxc::TextRenderer *textRenderer;
    };
}   // namespace m1
