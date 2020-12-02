#include "plugin.hpp"


struct Capacitor2 : Module {
	enum ParamIds {
		LONGPASS_PARAM,
		HIGHPASS_PARAM,
		NONLIN_PARAM,
		DRYWET_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_1_INPUT,
		INPUT_2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_2_OUTPUT,
		OUTPUT_1_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Capacitor2() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(LONGPASS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(HIGHPASS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(NONLIN_PARAM, 0.f, 1.f, 0.f, "");
		configParam(DRYWET_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct Capacitor2Widget : ModuleWidget {
	Capacitor2Widget(Capacitor2* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Capacitor2.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.657, 29.742)), module, Capacitor2::LONGPASS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.555, 44.551)), module, Capacitor2::HIGHPASS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.374, 60.507)), module, Capacitor2::NONLIN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.184, 76.421)), module, Capacitor2::DRYWET_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.819, 96.346)), module, Capacitor2::INPUT_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.64, 96.346)), module, Capacitor2::INPUT_2_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.682, 113.214)), module, Capacitor2::OUTPUT_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.846, 113.333)), module, Capacitor2::OUTPUT_1_OUTPUT));
	}
};


Model* modelCapacitor2 = createModel<Capacitor2, Capacitor2Widget>("Capacitor2");