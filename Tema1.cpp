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
    deckSize = 6;

    heartX = boardLeft - 170;
    playerHeartY = boardMiddle - 70;
    enemyHeartY = boardMiddle + 70;

    unitWidth = 9;
    unitHeight = 18;
    unitGap = 3;

    flipDuration = 0.25f;
    enemyStepDelay = 0.3f;
    dealDelay = 0.15f;

    draggedCard = -1;
    dealIndex = 0;

    float slotSpacing = (boardRight - boardLeft) / kCardsInHand;

    for (int i = 0; i < kCardsInHand; i++) {
        float slotX = boardLeft + (i + 0.5f) * slotSpacing;
        slots[0][i] = glm::vec2(slotX, (boardBottom + boardMiddle) / 2);
        slots[1][i] = glm::vec2(slotX, (boardMiddle + boardTop) / 2);
    }

    deckPositions[0] = glm::vec2(deckX, (boardBottom + boardMiddle) / 2);
    deckPositions[1] = glm::vec2(deckX, (boardMiddle + boardTop) / 2);

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

    Mesh* cardBorder = tema1Object2D::CreateRectangle("card_border", cardCorner, cardWidth, cardHeight, glm::vec3(0.1f, 0.1f, 0.1f));
    AddMeshToList(cardBorder);

    typeColors[kDamage] = glm::vec3(0.85f, 0.15f, 0.15f);
    typeColors[kHeal] = glm::vec3(0.15f, 0.75f, 0.25f);
    typeColors[kShield] = glm::vec3(0.15f, 0.35f, 0.90f);
    typeColors[kSpy] = glm::vec3(0.05f, 0.05f, 0.05f);
    typeColors[kFrost] = glm::vec3(0.75f, 0.85f, 0.95f);
    typeColors[kClear] = glm::vec3(0.95f, 0.85f, 0.10f);
    typeColors[kWarHorn] = glm::vec3(1.0f, 0.55f, 0.10f);
    typeColors[kScorch] = glm::vec3(0.60f, 0.20f, 0.80f);

    for (int type = 0; type < kTypeCount; type++) {
        glm::vec3 color = typeColors[type];
        glm::vec3 symbolColor = (type == kFrost) ? glm::vec3(0.45f, 0.65f, 0.90f) : color;
        string id = to_string(type);

        Mesh* stripe = tema1Object2D::CreateRectangle("stripe_" + id, stripeCorner, cardWidth, stripeHeight, color, true);
        AddMeshToList(stripe);

        Mesh* symbolTriangle = tema1Object2D::CreateTriangle("symbol_triangle_" + id, glm::vec3(0, 0, 0), 20, symbolColor, true);
        AddMeshToList(symbolTriangle);

        Mesh* symbolSquare = tema1Object2D::CreateSquare("symbol_square_" + id, glm::vec3(-8, -8, 0), 16, symbolColor, true);
        AddMeshToList(symbolSquare);

        Mesh* symbolBar = tema1Object2D::CreateRectangle("symbol_bar_" + id, glm::vec3(-11, -3, 0), 22, 6, symbolColor, true);
        AddMeshToList(symbolBar);
    }

    Mesh* valueSquare = tema1Object2D::CreateSquare("value_square", glm::vec3(-3, -3, 0), 6, glm::vec3(0.15f, 0.15f, 0.15f), true);
    AddMeshToList(valueSquare);

    Mesh* infinity =tema1Object2D::CreateInfinity("infinity", glm::vec3(0, 0, 0), 24, glm::vec3(0, 0, 0));
    AddMeshToList(infinity);

    Mesh* slotBorder = tema1Object2D::CreateRectangle("slot_border", cardCorner, cardWidth, cardHeight, glm::vec3(0.25f, 0.32f, 0.45f));
    AddMeshToList(slotBorder);

    Mesh* heart = tema1Object2D::CreateTriangle("heart", glm::vec3(0, 0, 0), 40, glm::vec3(0.9f, 0.1f, 0.15f), true);
    AddMeshToList(heart);

    Mesh* healthUnit = tema1Object2D::CreateRectangle("health_unit", glm::vec3(0, -unitHeight / 2, 0), unitWidth, unitHeight, glm::vec3(0.9f, 0.1f, 0.15f), true);
    AddMeshToList(healthUnit);

    Mesh* shieldUnit = tema1Object2D::CreateRectangle("shield_unit", glm::vec3(0, -unitHeight / 2, 0), unitWidth, unitHeight, glm::vec3(0.35f, 0.75f, 1.0f), true);
    AddMeshToList(shieldUnit);

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
    player = { 10, true, kCardsInHand, 0, false, false, 0, 0, 0 };
    enemy = { 10, !player.isTurn, kCardsInHand, 0, false, false, 0, 0, 0 };

    endTime = 0;

    for (int owner = 0; owner < 2; owner++) {
        for (int i = 0; i < kCardsInHand; i++) {
            Card &card = cards[owner * kCardsInHand + i];
            card.position = deckPositions[owner];
            card.targetPosition = card.position;
            card.handPosition = card.position;
            if (i < 3) {
                card.type = i;
            } else if (i < 7) {
                card.type = rand() % 3;
            } else {
                card.type = 3 + rand() % (kTypeCount - 3);
            }

            card.isPermanent = card.type == kDamage || card.type == kHeal || card.type == kShield;
            card.value = (card.type == kDamage || card.type == kScorch) ? 2 : 1;
            card.roundPlaced = -1;
            card.isCancelled = false;
            card.durability = card.isPermanent ? -1 : 3;
            card.scale = 1;
            card.angle = (owner == 1) ? glm::pi<float>() : 0;
            card.state.isFlipped = false;
            card.isOnTable = false;
            card.isHidden = (owner == 1);
            card.isFlipping = false;
            card.flipTime = 0;
            card.owner = owner;
            card.handSpot = i;
            card.slot = -1;
            card.isInDeck = true;
            card.isDestroyed = false;
            card.flipOnArrival = false;

            handSpotCard[owner][i] = owner * kCardsInHand + i;
            slotTaken[owner][i] = false;
        }
    }

    draggedCard = -1;
    dragOffset = glm::vec2(0, 0);
    pressPosition = glm::vec2(0, 0);

    dealIndex = 0;
    dealTimer = 0;

    enemyTimer = 0;
    enemyFlipIndex = 0;
    enemyPlacedCard = -1;
    roundNumber = 0;
}


void Tema1::FlipCard(Card &card)
{
    if (!card.isFlipping) {
        card.isFlipping = true;
        card.flipTime = 0;
    }
}


Tema1::Player &Tema1::GetPlayer(int owner)
{
    return (owner == 0) ? player : enemy;
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

    if (dealIndex == kTotalCards && player.isTurn && IsFinished(0) && IsFinished(1) &&
        !player.isDead && !enemy.isDead) {
        int playerTotal = player.health + player.shield;
        int enemyTotal = enemy.health + enemy.shield;

        if (playerTotal <= enemyTotal) {
            player.isDead = true;
        }

        if (enemyTotal <= playerTotal) {
            enemy.isDead = true;
        }
    }

    if (player.isDead || enemy.isDead) {
        endTime += deltaTimeSeconds;

        if (endTime > 10) {
            WorldStart();
        }

        return;
    }

    for (int i = 0; i < kTotalCards; i++) {
        UpdateCard(cards[i], deltaTimeSeconds);
    }

    if (dealIndex < kTotalCards) {
        UpdateDeal(deltaTimeSeconds);
    } else if (enemy.isTurn) {
        UpdateEnemyTurn(deltaTimeSeconds);
    } else if (IsFinished(0) && !IsFinished(1) && draggedCard == -1) {
        EndTurn();
    }

    RenderBoard();
    RenderSlots();
    RenderDeck(0);
    RenderDeck(1);
    RenderHearts();

    for (int i = 0; i < kTotalCards; i++) {
        if (i != draggedCard && cards[i].isOnTable) {
            RenderCard(cards[i]);
        }
    }

    for (int i = 0; i < kTotalCards; i++) {
        if (i != draggedCard && !cards[i].isOnTable) {
            RenderCard(cards[i]);
        }
    }

    if (draggedCard != -1) {
        RenderCard(cards[draggedCard]);
    }
}


void Tema1::FrameEnd()
{
    DrawHUD();
}


void Tema1::UpdateDeal(float deltaTimeSeconds)
{
    dealTimer += deltaTimeSeconds;
    if (dealTimer < dealDelay) {
        return;
    }
    dealTimer = 0;

    int owner = dealIndex % 2;
    int spot = dealIndex / 2;
    Card &card = cards[handSpotCard[owner][spot]];

    glm::vec2 deckTop = deckPositions[owner] + glm::vec2((deckSize - 1) * 2.0f, (deckSize - 1) * 2.0f);
    card.position = deckTop;
    card.isInDeck = false;
    card.flipOnArrival = (owner == 0);
    ArrangeHand(owner);

    dealIndex++;
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
    if (card.isInDeck || card.isDestroyed) {
        return;
    }

    glm::mat3 cardMatrix = glm::mat3(1);
    cardMatrix *= tema1Transform2D::Translate(card.position.x, card.position.y);
    cardMatrix *= tema1Transform2D::Rotate(card.angle);

    float flipScale = 1;

    if (card.isFlipping) {
        flipScale = glm::abs(glm::cos(glm::pi<float>() * card.flipTime / flipDuration));
    }

    cardMatrix *= tema1Transform2D::Scale(card.scale * flipScale, card.scale);

    if (!card.state.isFlipped) {
        if (card.owner == 1) {
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

    float stripeY[3] = { cardHeight / 2 - stripeHeight / 2, 0, -cardHeight / 2 + stripeHeight / 2 };

    for (int i = 0; i < 3; i++) {
        modelMatrix = cardMatrix;
        modelMatrix *= tema1Transform2D::Translate(0, stripeY[i]);
        RenderMesh2D(meshes["stripe_" + to_string(card.type)], shaders["VertexColor"], modelMatrix);
    }

    RenderSymbol(card.type, cardMatrix);

    for (int i = 0; i < card.value && i < 10; i++) {
        modelMatrix = cardMatrix;
        modelMatrix *= tema1Transform2D::Translate((i < 5) ? -33.0f : 33.0f, 46.0f - (i % 5) * 8);
        RenderMesh2D(meshes["value_square"], shaders["VertexColor"], modelMatrix);
    }

    if (card.isPermanent) {
        modelMatrix = cardMatrix;
        modelMatrix *= tema1Transform2D::Translate(0, -42);
        RenderMesh2D(meshes["infinity"], shaders["VertexColor"], modelMatrix);
    }

    RenderMesh2D(meshes["card_border"], shaders["VertexColor"], cardMatrix);

    if (card.isFlipping) {
        return;
    }

    RenderCardText(card, cardMatrix, "durability:", -22);

    if (!card.isPermanent) {
        RenderCardText(card, cardMatrix, to_string(card.durability), -37);
    }
}


void Tema1::RenderSymbol(int type, const glm::mat3 &cardMatrix)
{
    string id = to_string(type);
    glm::mat3 symbolMatrix = cardMatrix;
    symbolMatrix *= tema1Transform2D::Translate(0, 32);

    if (type == kDamage) {
        RenderMesh2D(meshes["symbol_triangle_" + id], shaders["VertexColor"], symbolMatrix);
    } else if (type == kHeal) {
        RenderMesh2D(meshes["symbol_bar_" + id], shaders["VertexColor"], symbolMatrix);
        modelMatrix = symbolMatrix;
        modelMatrix *= tema1Transform2D::Rotate(glm::pi<float>() / 2);
        RenderMesh2D(meshes["symbol_bar_" + id], shaders["VertexColor"], modelMatrix);
    } else if (type == kShield) {
        RenderMesh2D(meshes["symbol_square_" + id], shaders["VertexColor"], symbolMatrix);
    } else if (type == kSpy) {
        modelMatrix = symbolMatrix;
        modelMatrix *= tema1Transform2D::Rotate(glm::pi<float>() / 4);
        RenderMesh2D(meshes["symbol_square_" + id], shaders["VertexColor"], modelMatrix);
    } else if (type == kFrost) {
        modelMatrix = symbolMatrix;
        modelMatrix *= tema1Transform2D::Rotate(glm::pi<float>() / 4);
        RenderMesh2D(meshes["symbol_bar_" + id], shaders["VertexColor"], modelMatrix);
        modelMatrix = symbolMatrix;
        modelMatrix *= tema1Transform2D::Rotate(-glm::pi<float>() / 4);
        RenderMesh2D(meshes["symbol_bar_" + id], shaders["VertexColor"], modelMatrix);
    } else if (type == kClear) {
        modelMatrix = symbolMatrix;
        modelMatrix *= tema1Transform2D::Scale(1, -1);
        RenderMesh2D(meshes["symbol_triangle_" + id], shaders["VertexColor"], modelMatrix);
    } else if (type == kWarHorn) {
        modelMatrix = symbolMatrix;
        modelMatrix *= tema1Transform2D::Rotate(-glm::pi<float>() / 2);
        RenderMesh2D(meshes["symbol_triangle_" + id], shaders["VertexColor"], modelMatrix);
    } else if (type == kScorch) {
        modelMatrix = symbolMatrix;
        modelMatrix *= tema1Transform2D::Translate(0, -3);
        RenderMesh2D(meshes["symbol_triangle_" + id], shaders["VertexColor"], modelMatrix);
        modelMatrix = symbolMatrix;
        modelMatrix *= tema1Transform2D::Translate(0, 8);
        modelMatrix *= tema1Transform2D::Scale(0.5f, 0.5f);
        RenderMesh2D(meshes["symbol_triangle_" + id], shaders["VertexColor"], modelMatrix);
    }
}


void Tema1::RenderCardText(const Card &card, const glm::mat3 &cardMatrix, const string &text, float localY)
{
    float textWidth = 0;

    for (char c : text) {
        textWidth += (cardTextRenderer->Characters[c].Advance >> 6);
    }

    glm::vec3 textPosition = cardMatrix * glm::vec3(0, localY, 1);
    float textY = logicHeight - textPosition.y;

    if (card.angle != 0) {
        textY -= 10;
    }

    cardTextRenderer->RenderText(text, textPosition.x - textWidth / 2, textY, 1.0f, glm::vec3(0, 0, 0));
}


void Tema1::RenderSlots()
{
    for (int owner = 0; owner < 2; owner++) {
        for (int i = 0; i < kCardsInHand; i++) {
            modelMatrix = glm::mat3(1);
            modelMatrix *= tema1Transform2D::Translate(slots[owner][i].x, slots[owner][i].y);
            RenderMesh2D(meshes["slot_border"], shaders["VertexColor"], modelMatrix);
        }
    }
}


void Tema1::RenderDeck(int owner)
{
    string backName = (owner == 0) ? "card_back_player" : "card_back_enemy";

    for (int i = 0; i < deckSize; i++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= tema1Transform2D::Translate(deckPositions[owner].x + i * 2, deckPositions[owner].y + i * 2);
        RenderMesh2D(meshes[backName], shaders["VertexColor"], modelMatrix);
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
        string endText = "Win";

        if (player.isDead && enemy.isDead) {
            endText = "Draw";
        } else if (player.isDead) {
            endText = "Loss";
        }
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


void Tema1::PlaceCard(int index, int slot)
{
    Card &card = cards[index];
    Player &owner = GetPlayer(card.owner);

    handSpotCard[card.owner][card.handSpot] = -1;
    card.handSpot = -1;
    card.slot = slot;
    card.isOnTable = true;
    card.isHidden = false;
    card.targetPosition = slots[card.owner][slot];
    slotTaken[card.owner][slot] = true;
    card.roundPlaced = roundNumber;
    owner.cardsLeft--;

    if (owner.warHorn) {
        card.value *= 2;
        owner.warHorn = false;
    }

    ArrangeHand(card.owner);
}


void Tema1::ArrangeHand(int owner)
{
    int inHand[kCardsInHand];
    int inDeck[kCardsInHand];
    int n = 0;
    int m = 0;

    for (int i = 0; i < kCardsInHand; i++) {
        int index = handSpotCard[owner][i];

        if (index == -1) {
            continue;
        }

        if (cards[index].isInDeck) {
            inDeck[m++] = index;
        } else {
            inHand[n++] = index;
        }
    }

    for (int i = 0; i < kCardsInHand; i++) {
        handSpotCard[owner][i] = -1;
    }

    float handY = (owner == 0) ? playerHandY : enemyHandY;
    float rowWidth = n * cardWidth + (n - 1) * cardGap;
    float firstX = (boardLeft + boardRight) / 2 - rowWidth / 2 + cardWidth / 2;

    for (int k = 0; k < n; k++) {
        Card &card = cards[inHand[k]];
        handSpotCard[owner][k] = inHand[k];
        card.handSpot = k;
        card.handPosition = glm::vec2(firstX + k * (cardWidth + cardGap), handY);

        if (inHand[k] != draggedCard) {
            card.targetPosition = card.handPosition;
        }
    }

    for (int k = 0; k < m; k++) {
        handSpotCard[owner][n + k] = inDeck[k];
        cards[inDeck[k]].handSpot = n + k;
    }
}


void Tema1::DestroyCard(Card &card)
{
    card.isDestroyed = true;
    card.isOnTable = false;
    slotTaken[card.owner][card.slot] = false;
    card.slot = -1;
}


void Tema1::StealCard(int thief)
{
    int victim = 1 - thief;
    Player &victimPlayer = GetPlayer(victim);
    Player &thiefPlayer = GetPlayer(thief);

    int freeSpot = -1;

    for (int i = 0; i < kCardsInHand && freeSpot == -1; i++) {
        if (handSpotCard[thief][i] == -1) {
            freeSpot = i;
        }
    }

    if (victimPlayer.cardsLeft == 0 || freeSpot == -1) {
        return;
    }

    int n = rand() % victimPlayer.cardsLeft;
    int stolen = -1;

    for (int i = 0; i < kCardsInHand && stolen == -1; i++) {
        if (handSpotCard[victim][i] != -1) {
            if (n == 0) {
                stolen = handSpotCard[victim][i];
            }
            n--;
        }
    }

    Card &card = cards[stolen];

    if (stolen == draggedCard) {
        draggedCard = -1;
    }

    handSpotCard[victim][card.handSpot] = -1;
    handSpotCard[thief][freeSpot] = stolen;
    victimPlayer.cardsLeft--;
    thiefPlayer.cardsLeft++;

    card.owner = thief;
    card.handSpot = freeSpot;
    card.angle = (thief == 1) ? glm::pi<float>() : 0;
    card.isHidden = (thief == 1);

    bool faceUp = (thief == 0);

    if (card.state.isFlipped != faceUp) {
        FlipCard(card);
    }

    ArrangeHand(victim);
    ArrangeHand(thief);
}


void Tema1::ApplyCard(Card &card, Player &owner, Player &opponent)
{
    if (card.type == kScorch) {
        for (int i = 0; i < kTotalCards; i++) {
            Card &target = cards[i];

            if (target.isOnTable && target.owner != card.owner && target.roundPlaced == roundNumber && target.value >= card.value) {
                target.isCancelled = true;
            }
        }
    } else if (card.type == kSpy) {
        StealCard(card.owner);
    } else if (card.type == kClear) {
        for (int i = 0; i < kTotalCards; i++) {
            if (cards[i].isOnTable && cards[i].type == kFrost) {
                DestroyCard(cards[i]);
            }
        }
    } else if (card.type == kFrost) {
        for (int i = 0; i < kTotalCards; i++) {
            if (cards[i].isOnTable && cards[i].owner != card.owner) {
                cards[i].value--;
            }
        }
    } else if (card.type == kWarHorn) {
        owner.warHorn = true;
    } else if (card.type == kShield) {
        owner.shieldRegen += card.value;
    } else if (card.type == kDamage) {
        owner.damage += card.value;
    } else if (card.type == kHeal) {
        owner.regen += card.value;
    }
}


void Tema1::ApplyEffects()
{
    int order[8] = { kScorch, kSpy, kClear, kFrost, kWarHorn, kShield, kDamage, kHeal };

    for (int i = 0; i < kTotalCards; i++) {
        cards[i].isCancelled = false;
    }

    for (int owner = 0; owner < 2; owner++) {
        GetPlayer(owner).damage = 0;
        GetPlayer(owner).regen = 0;
        GetPlayer(owner).shieldRegen = 0;
    }

    for (int type : order) {
        for (int owner = 0; owner < 2; owner++) {
            for (int i = 0; i < kTotalCards; i++) {
                Card &card = cards[i];

                if (card.isOnTable && !card.isCancelled && card.owner == owner && card.type == type) {
                    ApplyCard(card, GetPlayer(owner), GetPlayer(1 - owner));
                }
            }
        }

        for (int i = 0; i < kTotalCards; i++) {
            if (cards[i].isOnTable && cards[i].value <= 0) {
                DestroyCard(cards[i]);
            }
        }

        for (int owner = 0; owner < 2; owner++) {
            Player &self = GetPlayer(owner);
            Player &opponent = GetPlayer(1 - owner);

            if (type == kShield) {
                self.shield = min(kMaxShield, self.shield + self.shieldRegen);
            } else if (type == kDamage) {
                int blocked = min(opponent.shield, self.damage);
                opponent.shield -= blocked;
                opponent.health = max(0, opponent.health - (self.damage - blocked));
            } else if (type == kHeal && !self.isDead) {
                self.health = min(kMaxHealth, self.health + self.regen);
            }
        }

        if (type == kDamage) {
            player.isDead = player.health <= 0;
            enemy.isDead = enemy.health <= 0;
        }
    }
}


void Tema1::EndRound()
{
    for (int i = 0; i < kTotalCards; i++) {
        Card &card = cards[i];

        if (card.isOnTable && !card.isPermanent) {
            card.durability--;

            if (card.durability <= 0) {
                DestroyCard(card);
            }
        }
    }

    roundNumber++;
}


void Tema1::UpdateEnemyTurn(float deltaTimeSeconds)
{
    if (enemyPlacedCard != -1) {
        Card &placed = cards[enemyPlacedCard];

        if (placed.flipOnArrival || placed.isFlipping) {
            return;
        }

        enemyPlacedCard = -1;
        ApplyEffects();
        EndRound();
        EndTurn();
        return;
    }

    enemyTimer += deltaTimeSeconds;
    if (enemyTimer < enemyStepDelay) {
        return;
    }
    enemyTimer = 0;

    while (enemyFlipIndex < kCardsInHand && handSpotCard[1][enemyFlipIndex] == -1) {
        enemyFlipIndex++;
    }

    if (enemyFlipIndex < kCardsInHand && !IsFinished(1)) {
        FlipCard(cards[handSpotCard[1][enemyFlipIndex]]);
        enemyFlipIndex++;
        return;
    }

    int chosenSlot = GetFirstFreeSlot(1);

    if (enemy.cardsLeft > 0 && chosenSlot != -1) {
        int n = rand() % enemy.cardsLeft;
        int chosenCard = -1;

        for (int i = 0; i < kCardsInHand && chosenCard == -1; i++) {
            if (handSpotCard[1][i] != -1) {
                if (n == 0) {
                    chosenCard = handSpotCard[1][i];
                }
                n--;
            }
        }

        PlaceCard(chosenCard, chosenSlot);

        Card &placed = cards[chosenCard];
        placed.isHidden = true;
        placed.flipOnArrival = true;

        if (placed.state.isFlipped) {
            FlipCard(placed);
        }

        enemyPlacedCard = chosenCard;
    }

    for (int i = 0; i < kCardsInHand; i++) {
        if (handSpotCard[1][i] != -1 && cards[handSpotCard[1][i]].state.isFlipped) {
            FlipCard(cards[handSpotCard[1][i]]);
        }
    }

    if (enemyPlacedCard == -1) {
        ApplyEffects();
        EndRound();
        EndTurn();
    }
}


int Tema1::GetFirstFreeSlot(int owner)
{
    for (int i = 0; i < kCardsInHand; i++) {
        if (!slotTaken[owner][i]) {
            return i;
        }
    }

    return -1;
}


bool Tema1::IsFinished(int owner)
{
    return GetPlayer(owner).cardsLeft == 0 || GetFirstFreeSlot(owner) == -1;
}


int Tema1::GetFreeSlotUnderMouse(glm::vec2 mousePosition)
{
    for (int i = 0; i < kCardsInHand; i++) {
        if (!slotTaken[0][i] &&
            mousePosition.x >= slots[0][i].x - cardWidth / 2 &&
            mousePosition.x <= slots[0][i].x + cardWidth / 2 &&
            mousePosition.y >= slots[0][i].y - cardHeight / 2 &&
            mousePosition.y <= slots[0][i].y + cardHeight / 2) {
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

    if (card.flipOnArrival && card.position == card.targetPosition && !card.isFlipping) {
        if (card.isOnTable) {
            card.isHidden = false;
        }

        FlipCard(card);
        card.flipOnArrival = false;
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
    if (key == GLFW_KEY_ENTER) {
        if (player.isDead || enemy.isDead) {
            endTime += 10;
        }
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    if (draggedCard != -1) {
        Card &card = cards[draggedCard];
        card.position = ScreenToWorld(mouseX, mouseY) - dragOffset;
        card.targetPosition = card.position;
    }
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && !player.isDead && !enemy.isDead && dealIndex == kTotalCards) {
        glm::vec2 mousePosition = ScreenToWorld(mouseX, mouseY);

        for (int i = 0; i < kCardsInHand; i++) {
            int index = handSpotCard[0][i];

            if (index != -1 && IsMouseOverCard(cards[index], mousePosition)) {
                draggedCard = index;
                dragOffset = mousePosition - cards[index].position;
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
        Card &card = cards[draggedCard];
        glm::vec2 mousePosition = ScreenToWorld(mouseX, mouseY);

        if (glm::length(mousePosition - pressPosition) < clickThreshold) {
            FlipCard(card);
            card.targetPosition = card.handPosition;
            draggedCard = -1;
            return;
        }

        int slot = GetFreeSlotUnderMouse(mousePosition);

        bool overBoard = mousePosition.x >= boardLeft && mousePosition.x <= boardRight &&
            mousePosition.y >= boardBottom && mousePosition.y <= boardTop;

        if (slot == -1 && overBoard) {
            slot = GetFirstFreeSlot(0);
        }

        if (player.isTurn && slot != -1) {
            PlaceCard(draggedCard, slot);
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
