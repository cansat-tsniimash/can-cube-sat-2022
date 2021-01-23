#include <ccsds/uslp/virtual/abstract_vchannel.hpp>

#include <sstream>

#include <ccsds/uslp/exceptions.hpp>
#include <ccsds/uslp/_detail/tf_header.hpp>


namespace ccsds { namespace uslp {



vchannel_source::vchannel_source(gvcid_t gvcid_)
	: gvcid(gvcid_)
{

}


void vchannel_source::frame_size_l2(uint16_t value)
{
	if (_finalized)
	{
		std::stringstream error;
		error << "unable to use frame_size_l2 on vchannel, because it is finalized";
		throw object_is_finalized(error.str());
	}

	_frame_size_l2 = value;
}


void vchannel_source::frame_seq_no_len(uint8_t len)
{
	if (_finalized)
	{
		std::stringstream error;
		error << "unable to use frame_seq_no_len on vchannel, because it is finalized";
		throw object_is_finalized(error.str());
	}

	_frame_seq_no.value(0, len);
}


void vchannel_source::add_map_source(map_source * source)
{
	if (_finalized)
	{
		std::stringstream error;
		error << "unable to use add_map_source on vchannel, because it is finalized";
		throw object_is_finalized(error.str());
	}

	add_map_source_impl(source);
}


void vchannel_source::finalize()
{
	if (_finalized)
		return;

	finalize_impl();
	_finalized = true;
}


bool vchannel_source::peek_frame()
{
	if (!_finalized)
	{
		std::stringstream error;
		error << "unable to use peek_frame() on vchannel, because it is not finalized";
		throw object_is_finalized(error.str());
	}

	return peek_frame_impl();
}


bool vchannel_source::peek_frame(vchannel_frame_params & params)
{
	if (!_finalized)
	{
		std::stringstream error;
		error << "unable to use peek_frame(params) on vchannel, because it is not finalized";
		throw object_is_finalized(error.str());
	}

	return peek_frame_impl(params);
}


void vchannel_source::pop_frame(uint8_t * tfdf_buffer)
{
	if (!_finalized)
	{
		std::stringstream error;
		error << "unable to use pop_frame() on vchannel, because it is not finalized";
		throw object_is_finalized(error.str());
	}

	pop_frame_impl(tfdf_buffer);
}


void vchannel_source::set_frame_seq_no(uint64_t value)
{
	_frame_seq_no.value(value, _frame_seq_no.value_size());
}


frame_seq_no_t vchannel_source::get_frame_seq_no() const
{
	return _frame_seq_no;
}


void vchannel_source::increase_frame_seq_no()
{
	_frame_seq_no++;
}


uint16_t vchannel_source::frame_size_overhead() const
{
	uint16_t retval = 0;
	retval += detail::tf_header_t::extended_size_forecast(frame_seq_no_len());

	return retval;
}


void vchannel_source::finalize_impl()
{
	const auto min_frame_size = frame_size_overhead();
	if (frame_size_l2() < min_frame_size)
	{
		std::stringstream error;
		error << "invalid frame size l2 on vchannel: " << frame_size_l2() << ". "
				<< "It should be no less than " << min_frame_size;
		throw einval_exception(error.str());
	}
}


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


vchannel_sink::vchannel_sink(gvcid_t gvcid_)
	: gvcid(gvcid_)
{

}


void vchannel_sink::add_map_sink(map_sink * sink)
{
	if (_finalized)
	{
		std::stringstream error;
		error << "unable to use add_map_sink() on vchannel, because it is finalized";
		throw object_is_finalized(error.str());
	}

	add_map_sink_impl(sink);
}


void vchannel_sink::finalize()
{
	if (_finalized)
		return;

	finalize_impl();
	_finalized = true;
}


void vchannel_sink::finalize_impl()
{
	// Ничего пока не делаем. Чую тут будут танцы с фармом
}


void vchannel_sink::push(
		const vchannel_frame_params & frame_params,
		const uint8_t * tfdf_buffer, uint16_t tfdf_buffer_size
)
{
	if (!_finalized)
	{
		std::stringstream error;
		error << "unable to use push() on vchannel, because it is not finalized";
		throw object_is_finalized(error.str());
	}

	push_impl(frame_params, tfdf_buffer, tfdf_buffer_size);
}




}}