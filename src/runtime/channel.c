
#include "tinygo.h"

void * __go_new_channel(ChanType *type, uintptr_t size) {
	channel_t *ch = tinygo_alloc(sizeof(channel_t));
	return ch;
}

void __go_send_small(ChanType *type, Hchan *ch_in, uint64_t value) {
	channel_t *ch = (channel_t*)ch_in;
	DEBUG_printf("-> value %d on channel %p\n", (int)value, ch);

	goroutine_work_counter++;
	// wait until the other goroutine has finished sending
	while (ch->state == CHAN_SENDING) {
		DEBUG_printf("-> wait for other goroutine to stop sending\n");
		tinygo_block();
	}

	goroutine_work_counter++;

	DEBUG_printf("-> send!\n");
	// put the value in the channel
	ch->state = CHAN_SENDING;
	ch->value = value;

	// wait until the receiver has received the value
	while (ch->state == CHAN_SENDING) {
		DEBUG_printf("-> wait for receiver to receive\n");
		tinygo_block();
	}
	if (ch->state == CHAN_CLOSED) {
		runtime_panicstring("send on closed channel");
	}
	DEBUG_printf("-> done\n");
}

bool chanrecv2(ChanType *type, Hchan *ch_in, byte *value_in) __asm__("runtime.chanrecv2");
bool chanrecv2(ChanType *type, Hchan *ch_in, byte *value_in)  {
	channel_t *ch = (channel_t*)ch_in;
	uint64_t *value = (uint64_t*)value_in;
	DEBUG_printf("<- receiving value from channel\n");
	goroutine_work_counter++;
	while (ch->state == CHAN_NULL) {
		DEBUG_printf("<- wait for value\n");
		tinygo_block();
	}
	if (ch->state == CHAN_SENDING) {
		ch->state = CHAN_NULL;
		*value = ch->value; // TODO what about bigger values?
		return true;
	} else { // CHAN_CLOSED
		*value = 0;
		return false;
	}
}

void __go_receive(ChanType *type, Hchan *ch, byte *value) {
	chanrecv2(type, ch, value);
}

void __go_builtin_close(Hchan *ch_in) {
	channel_t *ch = (channel_t*)ch_in;
	ch->state = CHAN_CLOSED;
}
