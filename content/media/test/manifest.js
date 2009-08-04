// In each list of tests below, test file types that are not supported should
// be ignored. To make sure tests respect that, we include a file of type
// "bogus/duh" in each list.

// These are small test files, good for just seeing if something loads.
var gSmallTests = [
  { name:"r11025_s16_c1.wav", type:"audio/x-wav", duration:1.0 },
  { name:"320x240.ogv", type:"video/ogg", width:320, height:240 },
  { name: "bug499519.ogv", type:"video/ogg", duration:0.24 },
  { name:"bogus.duh", type:"bogus/duh" }
];

// These are files that we just want to make sure we can play through.
// We can also check metadata.
// Put files of the same type together in this list so if something crashes
// we have some idea of which backend is responsible.
var gPlayTests = [
  // 8-bit samples
  { name:"r11025_u8_c1.wav", type:"audio/x-wav", duration:1.0 },
  // 8-bit samples, file is truncated
  { name:"r11025_u8_c1_trunc.wav", type:"audio/x-wav", duration:1.8 },
  // file has trailing non-PCM data
  { name:"r11025_s16_c1_trailing.wav", type:"audio/x-wav", duration:1.0 },
  // file with list chunk
  { name:"r16000_u8_c1_list.wav", type:"audio/x-wav", duration:4.2 },
  // Ogg stream with eof marker
  { name:"bug461281.ogg", type:"application/ogg" },
  // oggz-chop stream
  { name:"bug482461.ogv", type:"video/ogg", duration:4.24 },
  // With first frame a "duplicate" (empty) frame.
  { name:"bug500311.ogv", type:"video/ogg", duration:1.96 },
  // Small audio file
  { name:"small-shot.ogg", type:"video/ogg" },

  { name:"bogus.duh", type:"bogus/duh" }
];

// These are files that should refuse to play and report an error,
// without crashing of course.
// Put files of the same type together in this list so if something crashes
// we have some idea of which backend is responsible.
var gErrorTests = [
  { name: "bug498855-1.ogv", type:"video/ogg", duration:0.2 },
  { name: "bug498855-2.ogv", type:"video/ogg", duration:0.2 },
  { name: "bug498855-3.ogv", type:"video/ogg", duration:0.2 },
  { name:"bogus.wav", type:"audio/x-wav" },
  { name:"bogus.ogv", type:"video/ogg" },
  { name:"448636.ogv", type:"video/ogg" },
  { name:"bogus.duh", type:"bogus/duh" }
];

// These are files that have nontrivial duration and are useful for seeking within.
var gSeekTests = [
  { name:"r11025_s16_c1.wav", type:"audio/x-wav", duration:1.0 },
  { name:"seek.ogv", type:"video/ogg", duration:3.966 },
  { name:"bogus.duh", type:"bogus/duh", duration:123 }
];

// These are files suitable for using with a "new Audio" constructor.
var gAudioTests = [
  { name:"r11025_s16_c1.wav", type:"audio/x-wav", duration:1.0 },
  { name:"sound.ogg", type:"audio/ogg" },
  { name:"bogus.duh", type:"bogus/duh", duration:123 }
];

function checkMetadata(msg, e, test) {
  if (test.width) {
    is(e.videoWidth, test.width, msg + " video width");
  }
  if (test.height) {
    is(e.videoHeight, test.height, msg + " video height");
  }
  if (test.duration) {
    ok(Math.abs(e.duration - test.duration) < 0.1,
       msg + " duration should be around " + test.duration);
  }
}
