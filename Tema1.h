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
            bool isOnTable;
        };

        struct Player
        {
            int health;
            bool isTurn;
            int cardsLeft;
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
        void RenderSlots();
        void RenderDeck();
        void RenderHearts();
        void UpdateEnemyTurn(float deltaTimeSeconds);
        void EndTurn();
        int GetFreeSlotUnderMouse(glm::vec2 mousePosition);
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

        // The 9 slots on the table for each player
        glm::vec2 playerSlots[kCardsInHand];
        glm::vec2 enemySlots[kCardsInHand];
        bool playerSlotTaken[kCardsInHand];
        bool enemySlotTaken[kCardsInHand];

        // Simulated enemy turn: flips its cards one by one, then places a random one
        float enemyTimer;
        float enemyStepDelay;
        int enemyFlipIndex;

        // Colors
        const glm::vec3 kBackgroundColor = glm::vec3(0.05f, 0.07f, 0.12f);

        // The actual renderer object
        gfxc::TextRenderer *textRenderer;
    };
}   // namespace m1
