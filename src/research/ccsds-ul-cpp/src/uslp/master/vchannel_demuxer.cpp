#include <ccsds/uslp/master/vchannel_demuxer.hpp>

namespace ccsds { namespace uslp {


vchannel_demuxer::vchannel_demuxer(gvcid_t gvcid)
	: mchannel_sink(gvcid)
{

}


void vchannel_demuxer::add_vchannel_sink_impl(vchannel_sink * sink)
{
	_container.insert(std::make_pair(sink->gvcid, sink));
}


void vchannel_demuxer::finalize_impl()
{
	for (auto & pair: _container)
		pair.second->finalize();
}


void vchannel_demuxer::push_impl(
		const mchannel_frame_params_t & frame_params,
		const uint8_t * frame_data_unit, uint16_t frame_data_unit_size
)
{
	// Собственно просто ищем нужный map канал
	auto itt = _container.find(frame_params.channel_id);
	if (itt == _container.end())
	{
		// У нас нет такого мап канала! Как так то
		return;
	}

	// Формируем параметры фрейма
	vchannel_frame_params vparams;
	vparams.channel_id = frame_params.channel_id;
	vparams.frame_class = frame_params.frame_class;
	vparams.frame_seq_no = frame_params.frame_seq_no;
	vparams.frame_seq_no_length = frame_params.frame_seq_no_length;

	// Передаем фрейм адресату
	const uint8_t * tfdfd_buffer = frame_data_unit;
	const uint16_t tfdf_size = frame_data_unit_size - frame_params.ocf_present ? sizeof(uint32_t) : 0;
	itt->second->push(vparams, tfdfd_buffer, tfdf_size);
}


}}
