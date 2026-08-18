## Matches
Этот сервис предназначен для обработки результатов с r5_API

## Requirements
* C++ compiler
* CMake
* protobuf
* gRPC
* Docker

## gRPC API
Сервис имеет следующие RPC методы:
```proto
service MatchService {
  rpc AddToken(AddTokenRequest) returns (AddTokenResponse);
  rpc ListTokens(ListTokensRequest) returns (ListTokensResponse);
  rpc ListMatches(ListMatchesRequest) returns (ListMatchesResponse);
  rpc GetMatchStats(MatchStatsRequest) returns (MatchStatsResponse);
  rpc GetMatchDetail(MatchDetailRequest) returns (MatchDetailResponse);
  rpc GetPlayerStats(PlayerStatsRequest) returns (PlayerStatsResponse);
}
```
### AddToken
Добавляет новый stats_token в БД <br>
Request:
```proto
message AddTokenRequest {
  string activation = 1;
  string expiration = 2;
  string stats_token = 3;
  string admin_token = 4;
  string player_token = 5;
}
```
Response:
```proto
message AddTokenResponse {
  int32 token_id = 1;
  string message = 2;
}
```

### ListTokens
Возвращает уже имеющиеся stats_token
Поддерживает фильтры по активным токенам, страницами и количеству на странице <br>
Request:
```proto
message ListTokensRequest {
  bool only_active = 1;
  int32 page_size = 2;
  string page_token = 3;
}
```
Response:
```proto
message ListTokensResponse {
  repeated TokenInfo tokens = 1;
  string next_page_token = 2;
  int32 total_count = 3;
}
```
### ListMatches
Возвращает список матчей. Подерживает следующие фильтры:
* дата начала
* дата завершения
* карта
* игрок
* размер страницы
* конкретный токен
Request:
```proto
message ListMatchesRequest {
  string start_date = 1;
  string end_date = 2;
  string map_name = 3;
  string player_hash = 4;
  int32 page_size = 5;
  string page_token = 6;
}
```
Response:
```proto
message MatchSummary {
  int32 id = 1;
  string mid = 2;
  string map_name = 3;
  string match_start = 4;
  int32 player_count = 5;
}
```
### GetMatchDetail
Возвращает информацию о конкретном матче <br>
Request:
```proto
message MatchDetailRequest {
  int32 match_id = 1;
}
```
Response:
```proto
message PlayerResult {
  string player_name = 1;
  string character_name = 2;
  int32 kills = 3;
  int32 assists = 4;
  int32 knockdowns = 5;
  int32 damage_dealt = 6;
  int32 survival_time = 7;
  string team_name = 8;
  int32 team_placement = 9;
  string nid_hash = 10;
}
```
### GetMatchStats
Возвращает статистику по какому-то набору матчей. Поддерживаемые фильтры:
* Период
* Карта
* Минимальные киллы
* Максимальные киллы
* Игрок
Response:
```proto
message MatchStatsResponse {
  int32 total_matches = 1;
  int32 total_players = 2;
  double avg_kills = 3;
  double avg_damage = 4;
  double avg_survival_time = 5;
}
```

### GetPlayerStats
Возвращает статистику по конкретному игроку. Поддерживаемые фильтры:
* Игрок
* Период
Response:
```proto
message PlayerStatsResponse {
  string player_hash = 1;
  int32 matches_played = 2;
  int32 total_kills = 3;
  int32 total_damage = 4;
  double avg_damage = 5;
  double avg_survival_time = 6;
  int32 total_wins = 7;
  repeated CharacterStats top_characters = 8;
}
```
Character stats:
```proto
message CharacterStats {
  string character_name = 1;
  int32 matches = 2;
  int32 kills = 3;
  int32 picks = 4;
}
```
### Pagination
Список эндпоинтов для запросов по страницам
Если запрос содержит:
```
page_size
page_token
```
То ответ будет:
```
next_page_token
total_count
```