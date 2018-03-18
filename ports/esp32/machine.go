
package machine

func gpio_config(*gpio_config_t) uint __asm__("gpio_config")
func gpio_set_direction(uint, uint) uint __asm__("gpio_set_direction")
func gpio_set_level(uint, uint32) uint __asm__("gpio_set_level")
func gpio_get_level(uint) int __asm__("gpio_get_level")

type gpio_config_t struct {
	pin_bit_mask  uint64
	mode          uint8
	pull_up_en    uint8
	pull_down_en  uint8
	gpio_int_type uint8
}

const (
	GPIO_DISABLE    = 0
	GPIO_INPUT      = 1
	GPIO_OUTPUT     = 2
	GPIO_OPEN_DRAIN = 4
)

const (
	GPIO_PULLUP   = iota
	GPIO_PULLDOWN
	GPIO_PULLBOTH
	GPIO_FLOATING
)

type GPIO struct {
	pin uint
}

type GPIOConfig struct {
	Mode     uint8
	Pull     uint8 // TODO
}

func (p *GPIO) Configure(config GPIOConfig) {
	c := gpio_config_t{
		pin_bit_mask: 1 << p.pin,
		mode:         config.Mode,
	}
	gpio_config(&c)
}

func (p *GPIO) Set(value bool) {
	level := uint32(0)
	if value {
		level = 1
	}
	gpio_set_level(p.pin, level)
}

func (p *GPIO) Get() bool {
	level := gpio_get_level(p.pin)
	if level != 0 {
		return true
	}
	return false
}

// TODO: split the rest of this module into a board package.

var (
	GPIO2 = GPIO{2}
	GPIO4 = GPIO{4}
)

var LED_BUILTIN = GPIO2
