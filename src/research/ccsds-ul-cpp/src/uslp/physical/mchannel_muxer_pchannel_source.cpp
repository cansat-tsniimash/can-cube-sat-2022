#include <ccsds/uslp/physical/mchannel_muxer_pchannel_source.hpp>
#include <cassert>

namespace ccsds { namespace uslp {


mchannel_muxer_pchannel_source::mchannel_muxer_pchannel_source(std::string name_)
	: pchannel_source(std::move(name))
{

}


mchannel_muxer_pchannel_source::mchannel_muxer_pchannel_source(std::string name_, int32_t frame_size_)
	: pchannel_source(std::move(name_), frame_size_)
{

}


mchannel_muxer_pchannel_source::mchannel_muxer_pchannel_source(
		std::string name_, int32_t frame_size_, error_control_len_t err_control_len_
) : pchannel_source(std::move(name_), frame_size_, err_control_len_)
{

}


void mchannel_muxer_pchannel_source::frame_size(int32_t value)
{
	pchannel_source::frame_size(value);
}


bool mchannel_muxer_pchannel_source::peek_frame()
{
	if (!_selected_mchannel)
		_selected_mchannel = _muxer.select_next();

	if (_selected_mchannel)
		return false;

	return _selected_mchannel->peek_frame();
}


bool mchannel_muxer_pchannel_source::peek_frame(pchannel_frame_params_t & frame_params)
{
	if (!_selected_mchannel)
		_selected_mchannel = _muxer.select_next();

	if (_selected_mchannel)
		return false;

	mchannel_frame_params_t mchannel_params;
	bool retval = _selected_mchannel->peek_frame(mchannel_params);
	if (!retval)
		return false;

	frame_params.channel_id = mchannel_params.channel_id;
	frame_params.id_is_destination = mchannel_params.id_is_destination;
	frame_params.frame_class = mchannel_params.frame_class;
	frame_params.ocf_present = mchannel_params.ocf_present;
	frame_params.frame_seq_no = mchannel_params.frame_seq_no;
	frame_params.frame_seq_no_length = mchannel_params.frame_seq_no_length;

	return true;
}


void mchannel_muxer_pchannel_source::pop_frame(uint8_t * frame_buffer)
{
}


void mchannel_muxer_pchannel_source::attach_master_channel_source(mchannel_source * mchannel)
{
	mchannel->frame_data_unit_size(_frame_data_unit_size());
	_muxer.add_source(mchannel);
}


uint16_t mchannel_muxer_pchannel_source::_frame_data_unit_size()
{
	const auto frame_size = pchannel_source::frame_size();
	std::decay<decltype(frame_size)>::type retval = frame_size;

	// смотрим сколько байт мы отъедим на контрольную сумму
	switch (pchannel_source::error_control_len())
	{
	case error_control_len_t::ZERO: retval -= 0; break;
	case error_control_len_t::TWO_OCTETS: retval -= 2; break;
	case error_control_len_t::FOUR_OCTETS: retval -= 4; break;
	default:
		assert(false);
	};

	// TODO: сколько байт мы отъедим на insert сервис

	// Вот теперь собственно и все
	return static_cast<uint16_t>(retval);
}


void mchannel_muxer_pchannel_source::_sync_data_unit_size()
{
	const auto data_size = _frame_data_unit_size();

	for (auto itt = _muxer.begin(); itt != _muxer.end(); itt++)
		(*itt)->frame_data_unit_size(data_size);
}

}}


