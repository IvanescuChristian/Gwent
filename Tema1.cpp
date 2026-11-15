#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;


// The two players: health is shown next to the hearts, the enemy's isTurn is always our negated value
Tema1::Player player = { 10, true, 9 };
Tema1::Player enemy = { 10, !player.isTurn, 9 };


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
    clickThreshold = 5;

    deckX = boardRight + 30 + cardWidth / 2;
    deckY = boardMiddle;

    heartX = boardLeft - 90;
    playerHeartY = boardMiddle - 70;
    enemyHeartY = boardMiddle + 70;

    glm::vec3 cardCorner = glm::vec3(-cardWidth / 2, -cardHeight / 2, 0);

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

    Mesh* slotBorder = tema1Object2D::CreateRectangle("slot_border", cardCorner, cardWidth, cardHeight, glm::vec3(0.25f, 0.32f, 0.45f));
    AddMeshToList(slotBorder);

    Mesh* heart =tema1Object2D::CreateTriangle("heart", glm::vec3(0, 0, 0), 40, glm::vec3(0.9f, 0.1f, 0.15f), true);
    AddMeshToList(heart);

    // The hand: 9 cards, one next to another, centered under the board
    float handWidth = kCardsInHand * cardWidth + (kCardsInHand - 1) * cardGap;
    float firstCardX = (boardLeft + boardRight) / 2 - handWidth / 2 + cardWidth / 2;

    for (int i = 0; i < kCardsInHand; i++) {
        Card &card = playerCards[i];
        card.position = glm::vec2(firstCardX + i * (cardWidth + cardGap), playerHandY);
        card.targetPosition = card.position;
        card.handPosition = card.position;
        card.type = 0;
        card.value = 4;
        card.scale = 1;
        card.angle = 0;
        card.state.isFlipped = false;
        card.isOnTable = false;

        // Mirrored card in the opponent hand: same card, rotated 180 degrees
        Card &enemyCard = enemyCards[i];
        enemyCard = card;
        enemyCard.position = glm::vec2(card.position.x, enemyHandY);
        enemyCard.targetPosition = enemyCard.position;
        enemyCard.handPosition = enemyCard.position;
        enemyCard.angle = glm::pi<float>();
    }

    // The table: 9 slots per player, spread over the width of the board
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
    enemyStepDelay = 0.3f;
    enemyFlipIndex = 0;

    srand((unsigned int)time(NULL));

    // Create the text renderer object
    textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);

    textRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 24);
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

    // The dragged card is drawn last, so it stays on top of the others
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
    cardMatrix *= tema1Transform2D::Scale(card.scale, card.scale);

    // Not flipped: we see the backside. Flipped: we see the front side
    if (card.state.isFlipped) {
        RenderMesh2D(meshes["card_front"], shaders["VertexColor"], cardMatrix);
    } else if (isEnemy) {
        RenderMesh2D(meshes["card_back_enemy"], shaders["VertexColor"], cardMatrix);
    } else {
        RenderMesh2D(meshes["card_back_player"], shaders["VertexColor"], cardMatrix);
    }

    RenderMesh2D(meshes["card_border"], shaders["VertexColor"], cardMatrix);
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
    // Just a visual stack of cards, each one slightly offset from the one below
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
    // Our heart: the triangle mirrored, so it points downwards
    modelMatrix = glm::mat3(1);
    modelMatrix *= tema1Transform2D::Translate(heartX, playerHeartY);
    modelMatrix *= tema1Transform2D::Scale(1, -1);
    RenderMesh2D(meshes["heart"], shaders["VertexColor"], modelMatrix);

    // Enemy heart: flipped compared to ours, so it points upwards
    modelMatrix = glm::mat3(1);
    modelMatrix *= tema1Transform2D::Translate(heartX, enemyHeartY);
    RenderMesh2D(meshes["heart"], shaders["VertexColor"], modelMatrix);
}


void Tema1::DrawHUD()
{
    // The text renderer uses screen coordinates, with (0, 0) in the top-left corner
    glm::vec3 kTextColor = glm::vec3(1, 1, 1);
    float textX = heartX + 30;

    textRenderer->RenderText(to_string(player.health), textX, logicHeight - playerHeartY - 12, 1.0f, kTextColor);
    textRenderer->RenderText(to_string(enemy.health), textX, logicHeight - enemyHeartY - 12, 1.0f, kTextColor);

    // Whose turn it is, top right. Grey with alpha 0.5: the text renderer only takes an RGB color,
    // so the grey is blended by hand with the background it is drawn over
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


void Tema1::UpdateEnemyTurn(float deltaTimeSeconds)
{
    // One step every enemyStepDelay seconds
    enemyTimer += deltaTimeSeconds;
    if (enemyTimer < enemyStepDelay) {
        return;
    }
    enemyTimer = 0;

    // First the cards in hand flip one by one, left to right
    while (enemyFlipIndex < kCardsInHand && enemyCards[enemyFlipIndex].isOnTable) {
        enemyFlipIndex++;
    }

    if (enemyFlipIndex < kCardsInHand) {
        enemyCards[enemyFlipIndex].state.isFlipped = true;
        enemyFlipIndex++;
        return;
    }

    // Then a random card (1 in cardsLeft) goes into a random free slot
    if (enemy.cardsLeft > 0) {
        // Pick the n-th card still in hand and the m-th free slot (free slots = cards left in hand)
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
        enemySlotTaken[chosenSlot] = true;
        enemy.cardsLeft--;

        // The cards left in hand go back face down
        for (int i = 0; i < kCardsInHand; i++) {
            if (!enemyCards[i].isOnTable) {
                enemyCards[i].state.isFlipped = false;
            }
        }
    }

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
        // The card follows the cursor while it is dragged
        Card &card = playerCards[draggedCard];
        card.position = ScreenToWorld(mouseX, mouseY) - dragOffset;
        card.targetPosition = card.position;
    }
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 mousePosition = ScreenToWorld(mouseX, mouseY);

        // Drag: start only if we clicked on one of our cards in hand
        // (the enemy cards and the cards on the table can't be clicked)
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

        // Click (the mouse barely moved): the card switches sides
        if (glm::length(mousePosition - pressPosition) < clickThreshold) {
            card.state.isFlipped = !card.state.isFlipped;
            card.targetPosition = card.handPosition;
            draggedCard = -1;
            return;
        }

        // Drop on a free slot during our turn: the card goes on the table and the turn ends
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

        // Drop anywhere else: the card goes back to its place in our hand (UpdateCard moves it there)
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
