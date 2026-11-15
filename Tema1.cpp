#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;


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

    glm::vec3 cardCorner = glm::vec3(-cardWidth / 2, -cardHeight / 2, 0);

    Mesh* enemyBoard = tema1Object2D::CreateRectangle("enemy_board", glm::vec3(0, 0, 0), boardRight - boardLeft, boardTop - boardMiddle, glm::vec3(0.20f, 0.11f, 0.15f), true);
    AddMeshToList(enemyBoard);

    Mesh* playerBoard = tema1Object2D::CreateRectangle("player_board", glm::vec3(0, 0, 0), boardRight - boardLeft, boardMiddle - boardBottom, glm::vec3(0.10f, 0.16f, 0.27f), true);
    AddMeshToList(playerBoard);

    Mesh* boardBorder = tema1Object2D::CreateRectangle("board_border", glm::vec3(0, 0, 0), boardRight - boardLeft, boardTop - boardBottom, glm::vec3(0.25f, 0.32f, 0.45f));
    AddMeshToList(boardBorder);

    Mesh* cardDamage = tema1Object2D::CreateRectangle("card_damage", cardCorner, cardWidth, cardHeight, glm::vec3(0.82f, 0.22f, 0.24f), true);
    AddMeshToList(cardDamage);

    Mesh* cardBorder = tema1Object2D::CreateRectangle("card_border", cardCorner, cardWidth, cardHeight, glm::vec3(0.95f, 0.95f, 0.95f));
    AddMeshToList(cardBorder);

    Mesh* symbolDamage = tema1Object2D::CreateTriangle("symbol_damage", glm::vec3(0, 0, 0), 40, glm::vec3(1.0f, 0.45f, 0.45f), true);
    AddMeshToList(symbolDamage);

    Mesh* valueSquare = tema1Object2D::CreateSquare("value_square", glm::vec3(0, 0, 0), 10, glm::vec3(0.95f, 0.15f, 0.15f), true);
    AddMeshToList(valueSquare);

    card.position = glm::vec2(logicWidth / 2 - 65, playerHandY);
    card.targetPosition = card.position;
    card.type = 0;
    card.value = 4;
    card.handPosition = card.position;
    card.scale = 1;
    card.angle = 0;

    // Mirrored card in the opponent hand: same card, rotated 180 degrees
    enemyCard = card;
    enemyCard.position = glm::vec2(logicWidth / 2 - 65, enemyHandY);
    enemyCard.targetPosition = enemyCard.position;
    enemyCard.handPosition = enemyCard.position;
    enemyCard.angle = glm::pi<float>();

    isDragging = false;
    dragOffset = glm::vec2(0, 0);
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
    UpdateCard(card, deltaTimeSeconds);
    UpdateCard(enemyCard, deltaTimeSeconds);

    RenderBoard();
    RenderCard(enemyCard);
    RenderCard(card);
}


void Tema1::FrameEnd()
{
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


void Tema1::RenderCard(const Card &card)
{
    glm::mat3 cardMatrix = glm::mat3(1);
    cardMatrix *= tema1Transform2D::Translate(card.position.x, card.position.y);
    cardMatrix *= tema1Transform2D::Rotate(card.angle);
    cardMatrix *= tema1Transform2D::Scale(card.scale, card.scale);

    RenderMesh2D(meshes["card_damage"], shaders["VertexColor"], cardMatrix);
    RenderMesh2D(meshes["card_border"], shaders["VertexColor"], cardMatrix);

    modelMatrix = cardMatrix;
    modelMatrix *= tema1Transform2D::Translate(0, 10);
    RenderMesh2D(meshes["symbol_damage"], shaders["VertexColor"], modelMatrix);

    float squareSide = 10;
    float squareGap = 3;
    float rowWidth = card.value * squareSide + (card.value - 1) * squareGap;

    for (int i = 0; i < card.value; i++) {
        modelMatrix = cardMatrix;
        modelMatrix *= tema1Transform2D::Translate(-rowWidth / 2 + i * (squareSide + squareGap), -cardHeight / 2 + 8);
        RenderMesh2D(meshes["value_square"], shaders["VertexColor"], modelMatrix);
    }
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
    if (isDragging) {
        // The card follows the cursor while it is dragged
        card.position = ScreenToWorld(mouseX, mouseY) - dragOffset;
        card.targetPosition = card.position;
    }
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 mousePosition = ScreenToWorld(mouseX, mouseY);

        // Drag: start only if we clicked on our card
        if (IsMouseOverCard(card, mousePosition)) {
            isDragging = true;
            dragOffset = mousePosition - card.position;
        }
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && isDragging) {
        // Drop: the card goes back to its place in our hand (UpdateCard moves it there)
        isDragging = false;
        card.targetPosition = card.handPosition;
    }
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
