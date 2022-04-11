#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
}

void GameScene::Initialize() {
	textureHandle_ = TextureManager::Load("mario.jpg");
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	model_ = Model::Create();
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform2_.Initialize();
	worldTransform3_.Initialize();
	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() { 
	worldTransform_.scale_ = {5.0f, 5.0f, 5.0f};
	worldTransform_.rotation_ = {0.0f, (XM_PI / 1.0f) + value_, (XM_PI / 2.0f) + value_};
	worldTransform_.translation_ = {20.0f, 10.0f, 0.0f};
	worldTransform_.UpdateMatrix();

	worldTransform2_.scale_ = {1.0f, 5.0f, 1.0f};
	worldTransform2_.rotation_ = {0.0f, XM_PI / 1.0f, 0.0f};
	worldTransform2_.translation_ = {0.0f, 5.0f, 0.0f};
	worldTransform2_.UpdateMatrix();

	worldTransform3_.scale_ = {5.0f, 5.0f, 5.0f};
	worldTransform3_.rotation_ = {0.0f, XM_PI / 1.0f, 0.0f};
	worldTransform3_.translation_ = {-25.0f+speed_, 0.0f, 5.0f};
	worldTransform3_.UpdateMatrix();
	if (input_->TriggerKey(DIK_SPACE)) {
	}
	value_ += 0.01f;
	speed_ += 0.05f;
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);
	model_->Draw(worldTransform2_, viewProjection_, textureHandle_);
	model_->Draw(worldTransform3_, viewProjection_, textureHandle_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
