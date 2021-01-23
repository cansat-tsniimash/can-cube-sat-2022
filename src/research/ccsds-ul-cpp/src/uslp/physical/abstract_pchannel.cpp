#include <ccsds/uslp/physical/abstract_pchannel.hpp>

#include <sstream>
#include <cassert>

#include <ccsds/uslp/_detail/tf_header.hpp>
#include <ccsds/uslp/exceptions.hpp>


namespace ccsds { namespace uslp {


pchannel_source::pchannel_source(std::string name_)
		: name(name_), _frame_version_no(detail::tf_header_t::default_frame_version_no)
{

}


void pchannel_source::frame_version_no(uint8_t value)
{
	if (value > 0x0f)
	{
		std::stringstream error;
		error << "Invalid value for frame version no: " << value << ". "
				<< "Value should be in range [0, 0x0F].";
		throw einval_exception(error.str());
	}

	_frame_version_no = value;
}


void pchannel_source::frame_size(size_t value)
{
	if (_finalized)
	{
		std::stringstream error;
		error << "unable to set frame size for phys channel because it is finalized";
		throw object_is_finalized(error.str());
	}

	_frame_size = value;
}


void pchannel_source::error_control_len(error_control_len_t value)
{
	if (_finalized)
	{
		std::stringstream error;
		error << "unable to set error_control_len for phys channel because it is finalized";
		throw object_is_finalized(error.str());
	}

	_error_control_len = value;
}


void pchannel_source::add_mchannel_source(mchannel_source * source)
{
	if (_finalized)
	{
		std::stringstream error;
		error << "unable to add mchannel source to pchannel because pchannel is finalized";
		throw object_is_finalized(error.str());
	}

	add_mchannel_source_impl(source);
}


void pchannel_source::finalize()
{
	if (_finalized)
		return;

	finalize_impl();

	// Если выше будет бросок - мы до сюда не дойдем
	_finalized = true;
}


bool pchannel_source::peek()
{
	if (!_finalized)
	{
		std::stringstream error;
		error << "unable to use peek_frame() on virtual channel because it is finalized";
		throw object_is_finalized(error.str());
	}

	return peek_impl();
}


bool pchannel_source::peek(pchannel_frame_params_t & frame_params)
{
	if (!_finalized)
	{
		std::stringstream error;
		error << "unable to use peek_frame(frame_params) on physical channel because it is finalized";
		throw object_is_finalized(error.str());
	}

	return peek_impl(frame_params);
}


void pchannel_source::pop(uint8_t * frame_buffer, size_t frame_buffer_size)
{
	if (!_finalized)
	{
		std::stringstream error;
		error << "unable to use pop_frame() on physical channel because it is finalized";
		throw object_is_finalized(error.str());
	}

	assert(frame_size() <= frame_buffer_size);
	pop_impl(frame_buffer);
}


uint16_t pchannel_source::frame_size_overhead() const
{
	uint16_t retval = 0;

	switch (error_control_len())
	{
	case error_control_len_t::FOUR_OCTETS:
		retval += 4;
		break;

	case error_control_len_t::TWO_OCTETS:
		retval += 2;
		break;

	case error_control_len_t::ZERO:
		retval += 0;
		break;

	default:
		assert(false);
	};

	// TODO: Еще что-нибудь про инсерт зону

	return retval;
}


void pchannel_source::finalize_impl()
{
	// Проверяем самые базовые настройки

	// Смотрим сколько нам нужно минимально места на кадр
	// Прям совсем минимально.
	// Только чтобы влез заголовок и только
	const size_t minimum_frame_size = frame_size_overhead();
	// Максимально влезает только так - больше в заголовок не поместится
	const size_t maximum_frame_size = 0x10000;

	if (_frame_size < minimum_frame_size || _frame_size > maximum_frame_size)
	{
		std::stringstream error;
		error << "invalid value for frame_size: " << _frame_size << ". "
				<< "Value should be in range [" << minimum_frame_size << ", " << maximum_frame_size << "]";
		throw einval_exception(error.str());
	}
}


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

pchannel_sink::pchannel_sink(std::string name_)
	: name(std::move(name_))
{

}


void pchannel_sink::insert_zone_size(uint16_t value)
{
	if (_finalized)
	{
		std::stringstream error;
		error << "unable to use insert_zone_size(value) on physical channel because it is finalized";
		throw object_is_finalized(error.str());
	}

	_insert_zone_size = value;
}


void pchannel_sink::error_control_len(error_control_len_t value)
{
	if (_finalized)
	{
		std::stringstream error;
		error << "unable to use error_control_len(value) on physical channel because it is finalized";
		throw object_is_finalized(error.str());
	}

	_error_control_len = value;
}


void pchannel_sink::add_mchannel_sink(mchannel_sink * sink)
{
	if (_finalized)
	{
		std::stringstream error;
		error << "unable to use add_mchannel_sink(sink) on physical channel because it is finalized";
		throw object_is_finalized(error.str());
	}

	add_mchannel_sink_impl(sink);
}


void pchannel_sink::finalize()
{
	if (_finalized)
		return;

	finalize_impl();
	_finalized = true;
}


void pchannel_sink::push_frame(const uint8_t * frame_buffer, size_t frame_buffer_size)
{
	if (!_finalized)
	{
		std::stringstream error;
		error << "unable to use push_frame(frame_buffer) on physical channel because it is not finalized";
		throw object_is_finalized(error.str());
	}

	push_frame_impl(frame_buffer, frame_buffer_size);
}


void pchannel_sink::finalize_impl()
{
	// Ничего не делаем!
}



}}

