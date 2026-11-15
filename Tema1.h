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
            bool isHidden;
            bool isFlipping;
            float flipTime;
            bool isPermanent;
            int durability;
            int owner;
            int handSpot;
            int slot;
            bool isInDeck;
            bool isDestroyed;
            bool flipOnArrival;
            int roundPlaced;
            bool isCancelled;
            std::string name;
            bool isHorned;
            int effectValue;
        };

        struct Player
        {
            int health;
            bool isTurn;
            int cardsLeft;
            int shield;
            bool isDead;
            bool warHorn;
            int damage;
            int regen;
            int shieldRegen;
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

        void WorldStart();
        void FlipCard(Card &card);
        void RenderBoard();
        void RenderCard(const Card &card);
        void RenderSymbol(int type, const glm::mat3 &cardMatrix);
        void RenderCardText(const Card &card, const glm::mat3 &cardMatrix, const std::string &text, float localY);
        void RenderSlots();
        void RenderDeck(int owner);
        void RenderHearts();
        void RenderLifeBar(const Player &owner, float y, float shieldY);
        // Draw the heads-up display
        void DrawHUD();
        void UpdateCard(Card &card, float deltaTimeSeconds);
        void UpdateDeal(float deltaTimeSeconds);
        void UpdateEnemyTurn(float deltaTimeSeconds);
        void EndTurn();
        void PlaceCard(int index, int slot);
        void ArrangeHand(int owner);
        void DestroyCard(Card &card);
        void StealCard(int thief);
        void ApplyEffects();
        void ComputeEffectValues();
        void RenderCardName(const Card &card, const glm::mat3 &cardMatrix);
        void ApplyCard(Card &card, Player &owner, Player &opponent);
        void EndRound();
        Player &GetPlayer(int owner);
        int GetFreeSlotUnderMouse(glm::vec2 mousePosition);
        int GetFirstFreeSlot(int owner);
        bool IsFinished(int owner);
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
        static const int kTotalCards = 2 * kCardsInHand;
        static const int kTypeCount = 8;
        Card cards[kTotalCards];
        glm::vec3 typeColors[kTypeCount];

        static const int kDamage = 0;
        static const int kHeal = 1;
        static const int kShield = 2;
        static const int kSpy = 3;
        static const int kFrost = 4;
        static const int kClear = 5;
        static const int kWarHorn = 6;
        static const int kScorch = 7;
        static const int kMaxHealth = 10;
        static const int kMaxShield = 10;

        float flipDuration;
        float cardGap;
        float stripeHeight;
        float deckX;
        int deckSize;
        float heartX, playerHeartY, enemyHeartY;
        float unitWidth, unitHeight, unitGap;

        int draggedCard;
        glm::vec2 dragOffset;
        glm::vec2 pressPosition;
        float clickThreshold;

        int handSpotCard[2][kCardsInHand];
        glm::vec2 slots[2][kCardsInHand];
        bool slotTaken[2][kCardsInHand];
        glm::vec2 deckPositions[2];

        int dealIndex;
        float dealTimer;
        float dealDelay;

        float enemyTimer;
        float enemyStepDelay;
        int enemyFlipIndex;
        int enemyPlacedCard;
        int roundNumber;

        // Colors
        const glm::vec3 kBackgroundColor = glm::vec3(0.05f, 0.07f, 0.12f);

        // The actual renderer object
        gfxc::TextRenderer *textRenderer;
        gfxc::TextRenderer *cardTextRenderer;
        gfxc::TextRenderer *endTextRenderer;
    };
}   // namespace m1
