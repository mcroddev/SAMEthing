# samething

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

samething is a [Specific Area Message Encoding (SAME)](https://en.wikipedia.org/wiki/Specific_Area_Message_Encoding) header generator.

**WARNING**: The author is not responsible for the misuse of this software. The
output of this software has the potential to activate EAS decoder systems in the
continental United States of America and its territories, along with various
offshore marine areas, if transmitted.

Do not transmit the output of this software over any radio communication unless
so authorized.

Do not play the output of this software in public areas.

If you are under U.S. jurisdiction, please read [Misuse of the Emergency Alert System (EAS) Sound](https://www.fcc.gov/enforcement/areas/misuse-eas-sound) for more information.

## Example usage

If the user wants to generate a SAME header with this format:
`ZCZC-WXR-TOR-048487-048023+0100-1172112-KWFB/FM`

This is how one might accomplish this task:

```c
struct samething_core_header header = {
  .location_codes = {"048487", "048023", SAMETHING_CORE_LOCATION_CODE_END_MARKER},
  .originator_time = "1172112",
  .valid_time_period = "1000",
  .event_code = "TOR",
  .originator_code = "WXR",
  .attn_sig_duration = 8,
  .id = "KWFB/FM "
};

struct samething_core_gen_ctx ctx;
memset(&ctx, 0, sizeof(ctx));

samething_core_ctx_config(&ctx, &header);

while (num_samples < ctx.samples_num_max) {
  samething_core_samples_gen(&ctx);
  num_samples += SAMETHING_CORE_SAMPLES_NUM_MAX;
  push_samples_to_audio_device(ctx.sample_data);
}
```
