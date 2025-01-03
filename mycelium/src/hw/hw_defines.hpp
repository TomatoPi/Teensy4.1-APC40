/**
 * 
 */

#include "hw_defines.hxx"

namespace hw
{

template <>
struct row_type<pads::Pad>
    { using type = pads::PadRow; };

/**
 * Returns true if given pad has no led attached
 */
template <>
constexpr bool is_blind<pads::Pad>(pads::Pad p)
    { return p == pads::Pad::STOP_ALL_CLIPS || pads::Pad::__UNUSED_39_4__ < p; }


template <>
struct row_type<analog::Encoder>
    { using type = analog::EncoderGroup; };

/**
 * Returns true if given encoder has no led-ring attached
 */
template <>
constexpr bool is_blind<analog::Encoder>(analog::Encoder e)
    { return e == analog::Encoder::CUE_LEVEL; }


template <>
struct row_type<analog::Fader>
    { using type = analog::FaderGroup; };

/**
 * Returns true if given fader has no pads attached
 */
template <>
constexpr bool is_blind<analog::Fader>(analog::Fader f)
    { return f == analog::Fader::MASTER_LEVEL || f == analog::Fader::CROSSFADE; }


namespace pads
{


static_assert(static_cast<uint8_t>(Pad::__PADS_COUNT__) == 8 * static_cast<uint8_t>(PadRow::__ROWS_COUNT__));


static_assert(get_column(Pad::CLIP_0_0) == 0);
static_assert(get_column(Pad::CLIP_1_0) == 0);
static_assert(get_column(Pad::CLIP_2_0) == 0);
static_assert(get_column(Pad::CLIP_3_0) == 0);
static_assert(get_column(Pad::CLIP_4_0) == 0);

static_assert(get_column(Pad::CLIP_0_7) == 7);
static_assert(get_column(Pad::CLIP_1_7) == 7);
static_assert(get_column(Pad::CLIP_2_7) == 7);
static_assert(get_column(Pad::CLIP_3_7) == 7);
static_assert(get_column(Pad::CLIP_4_7) == 7);

static_assert(get_row(Pad::CLIP_0_0) == PadRow::Clip_0);
static_assert(get_row(Pad::CLIP_1_0) == PadRow::Clip_1);
static_assert(get_row(Pad::CLIP_2_0) == PadRow::Clip_2);
static_assert(get_row(Pad::CLIP_3_0) == PadRow::Clip_3);
static_assert(get_row(Pad::CLIP_4_0) == PadRow::Clip_4);

static_assert(get_row(Pad::CLIP_0_7) == PadRow::Clip_0);
static_assert(get_row(Pad::CLIP_1_7) == PadRow::Clip_1);
static_assert(get_row(Pad::CLIP_2_7) == PadRow::Clip_2);
static_assert(get_row(Pad::CLIP_3_7) == PadRow::Clip_3);
static_assert(get_row(Pad::CLIP_4_7) == PadRow::Clip_4);



static_assert(get_column(Pad::CLIP_STOP_0) == 0);
static_assert(get_column(Pad::CLIP_STOP_7) == 7);
static_assert(get_row(Pad::CLIP_STOP_0) == PadRow::ClipStop);
static_assert(get_row(Pad::CLIP_STOP_7) == PadRow::ClipStop);


static_assert(get_column(Pad::SCENE_LAUNCH_0) == 0);
static_assert(get_column(Pad::SCENE_LAUNCH_1) == 1);
static_assert(get_column(Pad::SCENE_LAUNCH_2) == 2);
static_assert(get_column(Pad::SCENE_LAUNCH_3) == 3);
static_assert(get_column(Pad::SCENE_LAUNCH_4) == 4);
static_assert(get_row(Pad::SCENE_LAUNCH_0) == PadRow::MasterControl);
static_assert(get_row(Pad::SCENE_LAUNCH_1) == PadRow::MasterControl);
static_assert(get_row(Pad::SCENE_LAUNCH_2) == PadRow::MasterControl);
static_assert(get_row(Pad::SCENE_LAUNCH_3) == PadRow::MasterControl);
static_assert(get_row(Pad::SCENE_LAUNCH_4) == PadRow::MasterControl);


static_assert(get_column(Pad::SELECT_MASTER) == 6);
static_assert(get_row(Pad::SELECT_MASTER) == PadRow::MasterControl);

static_assert(get_column(Pad::BANK_DOWN) == 7);
static_assert(get_row(Pad::BANK_DOWN) == PadRow::BankControl);


} /* endof namespace pads */
} /* endof namespace hw */
