#pragma once

/**
 * Deal `damage_per_tick` damage every `frequency` seconds over a total of `duration` seconds.
 */
struct CDamageOverTime {
	int damage_per_tick;
	float frequency;
	float duration;

	float _elapsed_since_tick = 0.f;
	float _elapsed_since_beginning = 0.f;

	CDamageOverTime(int damage_per_tick, float frequency, float duration)
	 : damage_per_tick{damage_per_tick}, frequency{frequency}, duration{duration}
	{

	}
};
