#include "com_coccoc_Tokenizer.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <tokenizer/tokenizer.hpp>

/*
 * Class:     com_coccoc_Tokenizer
 * Method:    segmentJson
 * Signature: (Ljava/lang/String;ZIZ)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_coccoc_Tokenizer_segmentJson(JNIEnv *env,
	jobject obj,
	jstring jni_text,
	jboolean for_transforming,
	jint tokenize_option,
	jboolean keep_puncts)
{
	// Use message-passing mechanism to transfer data to Java
	// return a Json string

	const char *original_text = env->GetStringUTFChars(jni_text, NULL);

	std::vector<FullToken> tokens = Tokenizer::instance().segment(original_text, for_transforming, tokenize_option, keep_puncts);
	env->ReleaseStringUTFChars(jni_text, original_text);

	// Serialize to json
	rapidjson::Document d;
	d.Parse("{\"tokens\": []}");

	for (size_t i = 0; i < tokens.size(); i++)
	{
		std::string key = "/tokens/" + std::to_string(i) + "/";

		rapidjson::Pointer((key + "originalStart").c_str()).Set(d, tokens[i].original_start);
		rapidjson::Pointer((key + "originalEnd").c_str()).Set(d, tokens[i].original_end);
		rapidjson::Pointer((key + "type").c_str()).Set(d, tokens[i].type);
		rapidjson::Pointer((key + "segType").c_str()).Set(d, tokens[i].seg_type);
		rapidjson::Pointer((key + "text").c_str()).Set(d, tokens[i].text.c_str());
	}

	// Serialize to Json
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);

	return env->NewStringUTF(buffer.GetString());
}

/*
 * Class:     com_coccoc_Tokenizer
 * Method:    initialize
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_coccoc_Tokenizer_initialize(JNIEnv *env, jobject obj, jstring jni_dict_path)
{
	const char *dict_path = env->GetStringUTFChars(jni_dict_path, nullptr);

	if (Tokenizer::instance().initialize(std::string(dict_path)) < 0) return -1;

	env->ReleaseStringUTFChars(jni_dict_path, dict_path);

	return 0;
}
