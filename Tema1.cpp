#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;


Tema1::Player player;
Tema1::Player enemy;

bool FirstTime = true;
float endTime = 0;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    logicWidth = (float)resolution.x;
    logicHeight = (float)resolution.y;

    cardWidth = 80;
    cardHeight = 120;

    boardLeft = 200;
    boardRight = 950;
    boardBottom = 240;
    boardMiddle = 380;
    boardTop = 520;

    playerHandY = 140;
    enemyHandY = 600;

    speed = 600;

    cardGap = 8;
    stripeHeight = 8;
    clickThreshold = 5;

    deckX = boardRight + 30 + cardWidth / 2;
    deckY = boardMiddle;

    heartX = boardLeft - 170;
    playerHeartY = boardMiddle - 70;
    enemyHeartY = boardMiddle + 70;

    unitWidth = 9;
    unitHeight = 18;
    unitGap = 3;

    glm::vec3 cardCorner = glm::vec3(-cardWidth / 2, -cardHeight / 2, 0);
    glm::vec3 stripeCorner = glm::vec3(-cardWidth / 2, -stripeHeight / 2, 0);

    Mesh* enemyBoard = tema1Object2D::CreateRectangle("enemy_board", glm::vec3(0, 0, 0), boardRight - boardLeft, boardTop - boardMiddle, glm::vec3(0.20f, 0.11f, 0.15f), true);
    AddMeshToList(enemyBoard);

    Mesh* playerBoard = tema1Object2D::CreateRectangle("player_board", glm::vec3(0, 0, 0), boardRight - boardLeft, boardMiddle - boardBottom, glm::vec3(0.10f, 0.16f, 0.27f), true);
    AddMeshToList(playerBoard);

    Mesh* boardBorder = tema1Object2D::CreateRectangle("board_border", glm::vec3(0, 0, 0), boardRight - boardLeft, boardTop - boardBottom, glm::vec3(0.25f, 0.32f, 0.45f));
    AddMeshToList(boardBorder);

    Mesh* cardFront = tema1Object2D::CreateRectangle("card_front", cardCorner, cardWidth, cardHeight, glm::vec3(1, 1, 1), true);
    AddMeshToList(cardFront);

    Mesh* cardBackPlayer = tema1Object2D::CreateRectangle("card_back_player", cardCorner, cardWidth, cardHeight, glm::vec3(0.15f, 0.35f, 0.85f), true);
    AddMeshToList(cardBackPlayer);

    Mesh* cardBackEnemy = tema1Object2D::CreateRectangle("card_back_enemy", cardCorner, cardWidth, cardHeight, glm::vec3(0.82f, 0.22f, 0.24f), true);
    AddMeshToList(cardBackEnemy);

    Mesh* cardBackDeck = tema1Object2D::CreateRectangle("card_back_deck", cardCorner, cardWidth, cardHeight, glm::vec3(0.35f, 0.35f, 0.40f), true);
    AddMeshToList(cardBackDeck);

    Mesh* cardBorder = tema1Object2D::CreateRectangle("card_border", cardCorner, cardWidth, cardHeight, glm::vec3(0.1f, 0.1f, 0.1f));
    AddMeshToList(cardBorder);

    Mesh* stripeDamage = tema1Object2D::CreateRectangle("stripe_damage", stripeCorner, cardWidth, stripeHeight, glm::vec3(0.85f, 0.15f, 0.15f), true);
    AddMeshToList(stripeDamage);

    Mesh* stripeHeal = tema1Object2D::CreateRectangle("stripe_heal", stripeCorner, cardWidth, stripeHeight, glm::vec3(0.15f, 0.75f, 0.25f), true);
    AddMeshToList(stripeHeal);

    Mesh* stripeShield = tema1Object2D::CreateRectangle("stripe_shield", stripeCorner, cardWidth, stripeHeight, glm::vec3(0.15f, 0.35f, 0.90f), true);
    AddMeshToList(stripeShield);

    Mesh* infinity = tema1Object2D::CreateInfinity("infinity", glm::vec3(0, 0, 0), 24, glm::vec3(0, 0, 0));
    AddMeshToList(infinity);

    Mesh* slotBorder = tema1Object2D::CreateRectangle("slot_border", cardCorner, cardWidth, cardHeight, glm::vec3(0.25f, 0.32f, 0.45f));
    AddMeshToList(slotBorder);

    Mesh* heart = tema1Object2D::CreateTriangle("heart", glm::vec3(0, 0, 0), 40, glm::vec3(0.9f, 0.1f, 0.15f), true);
    AddMeshToList(heart);

    Mesh* healthUnit = tema1Object2D::CreateRectangle("health_unit", glm::vec3(0, -unitHeight / 2, 0), unitWidth, unitHeight, glm::vec3(0.9f, 0.1f, 0.15f), true);
    AddMeshToList(healthUnit);

    Mesh* shieldUnit = tema1Object2D::CreateRectangle("shield_unit", glm::vec3(0, -unitHeight / 2, 0), unitWidth, unitHeight, glm::vec3(0.35f, 0.75f, 1.0f), true);
    AddMeshToList(shieldUnit);

    flipDuration = 0.25f;
    enemyStepDelay = 0.3f;

    srand((unsigned int)time(NULL));

    // Create the text renderer object
    textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);

    textRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 24);

    cardTextRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);

    cardTextRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 10);

    endTextRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);

    endTextRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 260);
}


void Tema1::WorldStart()
{
    player = { 10, true, 9, 0, false };
    enemy = { 10, !player.isTurn, 9, 0, false };

    endTime = 0;

    float handWidth = kCardsInHand * cardWidth + (kCardsInHand - 1) * cardGap;
    float firstCardX = (boardLeft + boardRight) / 2 - handWidth / 2 + cardWidth / 2;

    for (int i = 0; i < kCardsInHand; i++) {
        Card &card = playerCards[i];
        card.position = glm::vec2(firstCardX + i * (cardWidth + cardGap), playerHandY);
        card.targetPosition = card.position;
        card.handPosition = card.position;
        card.type = i % 3;
        card.value = (card.type == kDamage) ? 2 : 1;
        card.scale = 1;
        card.angle = 0;
        card.state.isFlipped = false;
        card.isOnTable = false;
        card.isHidden = false;
        card.isFlipping = false;
        card.flipTime = 0;

        Card &enemyCard = enemyCards[i];
        enemyCard = card;
        enemyCard.isHidden = true;
        enemyCard.position = glm::vec2(card.position.x, enemyHandY);
        enemyCard.targetPosition = enemyCard.position;
        enemyCard.handPosition = enemyCard.position;
        enemyCard.angle = glm::pi<float>();
    }

    float slotSpacing = (boardRight - boardLeft) / kCardsInHand;

    for (int i = 0; i < kCardsInHand; i++) {
        float slotX = boardLeft + (i + 0.5f) * slotSpacing;
        playerSlots[i] = glm::vec2(slotX, (boardBottom + boardMiddle) / 2);
        enemySlots[i] = glm::vec2(slotX, (boardMiddle + boardTop) / 2);
        playerSlotTaken[i] = false;
        enemySlotTaken[i] = false;
    }

    draggedCard = -1;
    dragOffset = glm::vec2(0, 0);
    pressPosition = glm::vec2(0, 0);

    enemyTimer = 0;
    enemyFlipIndex = 0;
}


void Tema1::FlipCard(Card &card)
{
    if (!card.isFlipping) {
        card.isFlipping = true;
        card.flipTime = 0;
    }
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(kBackgroundColor.r, kBackgroundColor.g, kBackgroundColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);

    glDisable(GL_DEPTH_TEST);
}


void Tema1::Update(float deltaTimeSeconds)
{
    if (FirstTime) {
        WorldStart();
        FirstTime = false;
    }

    if (player.health <= 0) {
        player.isDead = true;
    }

    if (enemy.health <= 0) {
        enemy.isDead = true;
    }

    if (player.isDead || enemy.isDead) {
        endTime += deltaTimeSeconds;

        if (endTime > 10) {
            WorldStart();
        }

        return;
    }

    for (int i = 0; i < kCardsInHand; i++) {
        UpdateCard(playerCards[i], deltaTimeSeconds);
        UpdateCard(enemyCards[i], deltaTimeSeconds);
    }

    if (enemy.isTurn) {
        UpdateEnemyTurn(deltaTimeSeconds);
    }

    RenderBoard();
    RenderSlots();
    RenderDeck();
    RenderHearts();

    for (int i = 0; i < kCardsInHand; i++) {
        RenderCard(enemyCards[i], true);
    }

    for (int i = 0; i < kCardsInHand; i++) {
        if (i != draggedCard) {
            RenderCard(playerCards[i], false);
        }
    }

    if (draggedCard != -1) {
        RenderCard(playerCards[draggedCard], false);
    }
}


void Tema1::FrameEnd()
{
    DrawHUD();
}


void Tema1::RenderBoard()
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= tema1Transform2D::Translate(boardLeft, boardMiddle);
    RenderMesh2D(meshes["enemy_board"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= tema1Transform2D::Translate(boardLeft, boardBottom);
    RenderMesh2D(meshes["player_board"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= tema1Transform2D::Translate(boardLeft, boardBottom);
    RenderMesh2D(meshes["board_border"], shaders["VertexColor"], modelMatrix);
}


void Tema1::RenderCard(const Card &card, bool isEnemy)
{
    glm::mat3 cardMatrix = glm::mat3(1);
    cardMatrix *= tema1Transform2D::Translate(card.position.x, card.position.y);
    cardMatrix *= tema1Transform2D::Rotate(card.angle);

    float flipScale = 1;

    if (card.isFlipping) {
        flipScale = glm::abs(glm::cos(glm::pi<float>() * card.flipTime / flipDuration));
    }

    cardMatrix *= tema1Transform2D::Scale(card.scale * flipScale, card.scale);

    if (!card.state.isFlipped) {
        if (isEnemy) {
            RenderMesh2D(meshes["card_back_enemy"], shaders["VertexColor"], cardMatrix);
        } else {
            RenderMesh2D(meshes["card_back_player"], shaders["VertexColor"], cardMatrix);
        }

        RenderMesh2D(meshes["card_border"], shaders["VertexColor"], cardMatrix);
        return;
    }

    RenderMesh2D(meshes["card_front"], shaders["VertexColor"], cardMatrix);

    if (card.isHidden) {
        RenderMesh2D(meshes["card_border"], shaders["VertexColor"], cardMatrix);
        return;
    }

    string stripeNames[3] = { "stripe_damage", "stripe_heal", "stripe_shield" };
    float stripeY[3] = { cardHeight / 2 - stripeHeight / 2, 0, -cardHeight / 2 + stripeHeight / 2 };

    for (int i = 0; i < 3; i++) {
        modelMatrix = cardMatrix;
        modelMatrix *= tema1Transform2D::Translate(0, stripeY[i]);
        RenderMesh2D(meshes[stripeNames[card.type]], shaders["VertexColor"], modelMatrix);
    }

    modelMatrix = cardMatrix;
    modelMatrix *= tema1Transform2D::Translate(0, -42);
    RenderMesh2D(meshes["infinity"], shaders["VertexColor"], modelMatrix);

    RenderMesh2D(meshes["card_border"], shaders["VertexColor"], cardMatrix);

    if (card.isFlipping) {
        return;
    }

    string durabilityText = "durability:";
    float textWidth = 0;

    for (char c : durabilityText) {
        textWidth += (cardTextRenderer->Characters[c].Advance >> 6);
    }

    glm::vec3 textPosition = cardMatrix * glm::vec3(0, -22, 1);
    float textY = logicHeight - textPosition.y;

    if (isEnemy) {
        textY -= 10;
    }

    cardTextRenderer->RenderText(durabilityText, textPosition.x - textWidth / 2, textY, 1.0f, glm::vec3(0, 0, 0));
}


void Tema1::RenderSlots()
{
    for (int i = 0; i < kCardsInHand; i++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= tema1Transform2D::Translate(playerSlots[i].x, playerSlots[i].y);
        RenderMesh2D(meshes["slot_border"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= tema1Transform2D::Translate(enemySlots[i].x, enemySlots[i].y);
        RenderMesh2D(meshes["slot_border"], shaders["VertexColor"], modelMatrix);
    }
}


void Tema1::RenderDeck()
{
    int deckSize = 6;

    for (int i = 0; i < deckSize; i++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= tema1Transform2D::Translate(deckX + i * 2, deckY + i * 2);
        RenderMesh2D(meshes["card_back_deck"], shaders["VertexColor"], modelMatrix);
        RenderMesh2D(meshes["card_border"], shaders["VertexColor"], modelMatrix);
    }
}


void Tema1::RenderHearts()
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= tema1Transform2D::Translate(heartX, playerHeartY);
    modelMatrix *= tema1Transform2D::Scale(1, -1);
    RenderMesh2D(meshes["heart"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= tema1Transform2D::Translate(heartX, enemyHeartY);
    RenderMesh2D(meshes["heart"], shaders["VertexColor"], modelMatrix);

    RenderLifeBar(player, playerHeartY, playerHeartY - unitHeight - 6);
    RenderLifeBar(enemy, enemyHeartY, enemyHeartY + unitHeight + 6);
}


void Tema1::RenderLifeBar(const Player &owner, float y, float shieldY)
{
    float barX = heartX + 30;

    for (int i = 0; i < owner.health; i++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= tema1Transform2D::Translate(barX + i * (unitWidth + unitGap), y);
        RenderMesh2D(meshes["health_unit"], shaders["VertexColor"], modelMatrix);
    }

    for (int i = 0; i < owner.shield; i++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= tema1Transform2D::Translate(barX + i * (unitWidth + unitGap), shieldY);
        RenderMesh2D(meshes["shield_unit"], shaders["VertexColor"], modelMatrix);
    }
}


void Tema1::DrawHUD()
{
    if (player.isDead || enemy.isDead) {
        string endText = player.isDead ? "Loss" : "Win";
        float textWidth = 0;

        for (char c : endText) {
            textWidth += (endTextRenderer->Characters[c].Advance >> 6);
        }

        float textHeight = (float)endTextRenderer->Characters['H'].Size.y;

        endTextRenderer->RenderText(endText, logicWidth / 2 - textWidth / 2, logicHeight / 2 - textHeight / 2, 1.0f, glm::vec3(1, 1, 1));
        return;
    }

    glm::vec3 kGrey = glm::vec3(0.5f, 0.5f, 0.5f);
    float kAlpha = 0.5f;
    glm::vec3 kTurnColor = kAlpha * kGrey + (1 - kAlpha) * kBackgroundColor;
    string turnText = player.isTurn ? "Players turn" : "Enemys turn";

    textRenderer->RenderText(turnText, logicWidth - 200, 20, 1.0f, kTurnColor);
}


void Tema1::EndTurn()
{
    player.isTurn = !player.isTurn;
    enemy.isTurn = !player.isTurn;

    enemyTimer = 0;
    enemyFlipIndex = 0;
}


void Tema1::ApplyCard(const Card &card, Player &owner, Player &opponent)
{
    if (card.type == kDamage) {
        int blocked = min(opponent.shield, card.value);
        opponent.shield -= blocked;
        opponent.health = max(0, opponent.health - (card.value - blocked));
    } else if (card.type == kHeal) {
        owner.health = min(kMaxHealth, owner.health + card.value);
    } else if (card.type == kShield) {
        owner.shield = min(kMaxShield, owner.shield + card.value);
    }
}


void Tema1::ApplyEffects()
{
    int order[3] = { kShield, kDamage, kHeal };

    for (int type : order) {
        for (int i = 0; i < kCardsInHand; i++) {
            if (playerCards[i].isOnTable && playerCards[i].type == type) {
                ApplyCard(playerCards[i], player, enemy);
            }
        }

        for (int i = 0; i < kCardsInHand; i++) {
            if (enemyCards[i].isOnTable && enemyCards[i].type == type) {
                ApplyCard(enemyCards[i], enemy, player);
            }
        }
    }
}


void Tema1::UpdateEnemyTurn(float deltaTimeSeconds)
{
    enemyTimer += deltaTimeSeconds;
    if (enemyTimer < enemyStepDelay) {
        return;
    }
    enemyTimer = 0;

    while (enemyFlipIndex < kCardsInHand && enemyCards[enemyFlipIndex].isOnTable) {
        enemyFlipIndex++;
    }

    if (enemyFlipIndex < kCardsInHand) {
        FlipCard(enemyCards[enemyFlipIndex]);
        enemyFlipIndex++;
        return;
    }

    if (enemy.cardsLeft > 0) {
        int n = rand() % enemy.cardsLeft;
        int m = rand() % enemy.cardsLeft;
        int chosenCard = -1;
        int chosenSlot = -1;

        for (int i = 0; i < kCardsInHand && chosenCard == -1; i++) {
            if (!enemyCards[i].isOnTable) {
                if (n == 0) {
                    chosenCard = i;
                }
                n--;
            }
        }

        for (int i = 0; i < kCardsInHand && chosenSlot == -1; i++) {
            if (!enemySlotTaken[i]) {
                if (m == 0) {
                    chosenSlot = i;
                }
                m--;
            }
        }

        Card &card = enemyCards[chosenCard];
        card.targetPosition = enemySlots[chosenSlot];
        card.isOnTable = true;
        card.isHidden = false;
        enemySlotTaken[chosenSlot] = true;
        enemy.cardsLeft--;

        for (int i = 0; i < kCardsInHand; i++) {
            if (!enemyCards[i].isOnTable) {
                FlipCard(enemyCards[i]);
            }
        }
    }

    ApplyEffects();
    EndTurn();
}


int Tema1::GetFreeSlotUnderMouse(glm::vec2 mousePosition)
{
    for (int i = 0; i < kCardsInHand; i++) {
        if (!playerSlotTaken[i] &&
            mousePosition.x >= playerSlots[i].x - cardWidth / 2 &&
            mousePosition.x <= playerSlots[i].x + cardWidth / 2 &&
            mousePosition.y >= playerSlots[i].y - cardHeight / 2 &&
            mousePosition.y <= playerSlots[i].y + cardHeight / 2) {
            return i;
        }
    }

    return -1;
}


void Tema1::UpdateCard(Card &card, float deltaTimeSeconds)
{
    glm::vec2 direction = card.targetPosition - card.position;
    float distance = glm::length(direction);
    float step = speed * deltaTimeSeconds;

    if (distance <= step) {
        card.position = card.targetPosition;
    } else {
        card.position += direction / distance * step;
    }

    if (card.isFlipping) {
        float previousTime = card.flipTime;
        card.flipTime += deltaTimeSeconds;

        if (previousTime < flipDuration / 2 && card.flipTime >= flipDuration / 2) {
            card.state.isFlipped = !card.state.isFlipped;
        }

        if (card.flipTime >= flipDuration) {
            card.isFlipping = false;
            card.flipTime = 0;
        }
    }
}


glm::vec2 Tema1::ScreenToWorld(int mouseX, int mouseY)
{
    glm::ivec2 resolution = window->GetResolution();
    return glm::vec2(mouseX * logicWidth / resolution.x, logicHeight - mouseY * logicHeight / resolution.y);
}


bool Tema1::IsMouseOverCard(const Card &card, glm::vec2 mousePosition)
{
    glm::vec2 halfSize = glm::vec2(cardWidth / 2, cardHeight / 2) * card.scale;

    return mousePosition.x >= card.position.x - halfSize.x &&
        mousePosition.x <= card.position.x + halfSize.x &&
           mousePosition.y >= card.position.y - halfSize.y &&
        mousePosition.y <= card.position.y + halfSize.y;
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    if (draggedCard != -1) {
        Card &card = playerCards[draggedCard];
        card.position = ScreenToWorld(mouseX, mouseY) - dragOffset;
        card.targetPosition = card.position;
    }
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && !player.isDead && !enemy.isDead) {
        glm::vec2 mousePosition = ScreenToWorld(mouseX, mouseY);

        for (int i = 0; i < kCardsInHand; i++) {
            if (!playerCards[i].isOnTable && IsMouseOverCard(playerCards[i], mousePosition)) {
                draggedCard = i;
                dragOffset = mousePosition - playerCards[i].position;
                pressPosition = mousePosition;
                break;
            }
        }
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && draggedCard != -1) {
        Card &card = playerCards[draggedCard];
        glm::vec2 mousePosition = ScreenToWorld(mouseX, mouseY);

        if (glm::length(mousePosition - pressPosition) < clickThreshold) {
            FlipCard(card);
            card.targetPosition = card.handPosition;
            draggedCard = -1;
            return;
        }

        int slot = GetFreeSlotUnderMouse(mousePosition);

        if (player.isTurn && slot != -1) {
            card.targetPosition = playerSlots[slot];
            card.isOnTable = true;
            playerSlotTaken[slot] = true;
            player.cardsLeft--;
            draggedCard = -1;
            EndTurn();
            return;
        }

        card.targetPosition = card.handPosition;
        draggedCard = -1;
    }
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
