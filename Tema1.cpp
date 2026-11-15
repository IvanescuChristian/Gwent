#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;


// Lives of each player, shown next to the hearts
int playerLives = 10;
int enemyLives = 10;


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

    Mesh* heart = tema1Object2D::CreateTriangle("heart", glm::vec3(0, 0, 0), 40, glm::vec3(0.9f, 0.1f, 0.15f), true);
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

        // Mirrored card in the opponent hand: same card, rotated 180 degrees
        Card &enemyCard = enemyCards[i];
        enemyCard = card;
        enemyCard.position = glm::vec2(card.position.x, enemyHandY);
        enemyCard.targetPosition = enemyCard.position;
        enemyCard.handPosition = enemyCard.position;
        enemyCard.angle = glm::pi<float>();
    }

    draggedCard = -1;
    dragOffset = glm::vec2(0, 0);
    pressPosition = glm::vec2(0, 0);

    // Create the text renderer object
    textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);

    textRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 24);
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.05f, 0.07f, 0.12f, 1);
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

    RenderBoard();
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

    textRenderer->RenderText(to_string(playerLives), textX, logicHeight - playerHeartY - 12, 1.0f, kTextColor);
    textRenderer->RenderText(to_string(enemyLives), textX, logicHeight - enemyHeartY - 12, 1.0f, kTextColor);
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

        // Drag: start only if we clicked on one of our cards (the enemy cards can't be clicked)
        for (int i = 0; i < kCardsInHand; i++) {
            if (IsMouseOverCard(playerCards[i], mousePosition)) {
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
        }

        // Drop: the card goes back to its place in our hand (UpdateCard moves it there)
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
