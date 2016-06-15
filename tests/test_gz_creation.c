/*
 *   Copyright (c) 2016 Kulykov Oleh <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#include "libnhr_public_tests.h"

#if defined(NHR_GZIP)

// Lorem Ipsum
const char * test_gz_string1 = "Text files are files containing sequences of lines of text. Depending on the environment where the application runs, some special character conversion may occur in input/output operations in text mode to adapt them to a system-specific text file format. Although on some environments no conversions occur and both text files and binary files are treated the same way, using the appropriate mode improves portability"
"\n" "Text files are files containing sequences of lines of text. Depending on the environment where the application runs, some special character conversion may occur in input/output operations in text mode to adapt them to a system-specific text file format. Although on some environments no conversions occur and both text files and binary files are treated the same way, using the appropriate mode improves portability"
"\n" "Text files are files containing sequences of lines of text. Depending on the environment where the application runs, some special character conversion may occur in input/output operations in text mode to adapt them to a system-specific text file format. Although on some environments no conversions occur and both text files and binary files are treated the same way, using the appropriate mode improves portability"
"\n" "Text files are files containing sequences of lines of text. Depending on the environment where the application runs, some special character conversion may occur in input/output operations in text mode to adapt them to a system-specific text file format. Although on some environments no conversions occur and both text files and binary files are treated the same way, using the appropriate mode improves portability"
"I motsättning till vad många tror, är inte Lorem Ipsum slumpvisa ord. Det har sina rötter i ett stycke klassiskt litteratur på latin från 45 år före år 0, och är alltså över 2000 år gammalt. Richard McClintock, en professor i latin på Hampden-Sydney College i Virginia, översatte ett av de mer ovanliga orden"
"Ao contrário da crença popular, o Lorem Ipsum não é simplesmente texto aleatório. Tem raízes numa peça de literatura clássica em Latim, de 45 AC, tornando-o com mais de 2000 anos. Richard McClintock, um professor de Latim no Colégio Hampden-Sydney, na Virgínia, procurou uma das palavras em Latim mais obscuras (consectetur) numa passagem Lorem Ipsum, e atravessando as cidades do mundo na literatura clássica, descobriu a sua origem. Lorem Ipsum vem das secções"
"не е случајно избран и сложен текст, спротивно од верувањата. Неговите корени потекнуваат во дела на класичната Латинска книжевност од 45-та година пред новата ера, што го прави стар преку 2000 години. Richard McClintock, професор по Латински на колеџот Hampden-Sydney во Вирџинија побарал дефиниција за малку чудните зборови consectetur од пасусите на Lorem Ipsum и додека ги анализирал деловите во класичната книжевност открил автентичен извор. Lorem Ipsum доаѓа од поглавијата"
"გავრცელებული მოსაზრებით, Lorem Ipsum შემთხვევითი ტექსტი სულაც არაა. მისი ფესვები ჯერკიდევ ჩვ. წ. აღ-მდე 45 წლის დროინდელი კლასიკური ლათინური ლიტერატურიდან მოდის. ვირჯინიის შტატში მდებარე ჰემპდენ-სიდნეის კოლეჯის პროფესორმა რიჩარდ მაკკლინტოკმა აიღო ერთ-ერთი ყველაზე იშვიათი ლათინური სიტყვა \"consectetur\" Lorem Ipsum-პასაჟიდან და გადაწყვიტა მოეძებნა იგი კლასიკურ ლიტერატურაში. ძიება შედეგიანი აღმოჩნდა — ტექსტი"
"На відміну від поширеної думки Lorem Ipsum не є випадковим набором літер. Він походить з уривку класичної латинської літератури 45 року до н.е., тобто має більш як 2000-річну історію. Річард Макклінток, професор латини з коледжу Хемпдін-Сидні, що у Вірджінії, вивчав одне з найменш зрозумілих латинських слів - consectetur - з уривку Lorem Ipsum, і у пошуку цього слова в класичній літературі знайшов безсумнівне джерело"
"Αντίθετα με αυτό που θεωρεί η πλειοψηφία, το Lorem Ipsum δεν είναι απλά ένα τυχαίο κείμενο. Οι ρίζες του βρίσκονται σε ένα κείμενο Λατινικής λογοτεχνίας του 45 π.Χ., φτάνοντας την ηλικία του πάνω από 2000 έτη. Ο Richard McClintock, καθηγητής Λατινικών στο κολλέγιο Hampden-Dydney στην Βιρτζίνια, αναζήτησε μία από τις πιο σπάνιες Λατινικές λέξεις, την consectetur, από ένα απόσπασμα του Lorem Ipsum, και ανάμεσα σε όλα τα έργα της κλασσικής λογοτεχνίας, ανακάλυψε την αναμφισβήτητη πηγή του"
"În ciuda opiniei publice, Lorem Ipsum nu e un simplu text fără sens. El îşi are rădăcinile într-o bucată a literaturii clasice latine din anul 45 î.e.n., făcând-o să aibă mai bine de 2000 ani. Profesorul universitar de latină de la colegiul Hampden-Sydney din Virginia, Richard McClintock, a căutat în bibliografie unul din cele"
"Հակառակ ընդհանուր պատկերացմանը` Lorem Ipsum-ը այդքան էլ պատահական հավաքված տեքստ չէ: Այս տեքստի արմատները հասնում են Ք.ա. 45թ. դասական լատինական գրականություն. այն 2000 տարեկան է: Ռիչարդ ՄքՔլինտոքը` Վիրջինիայի Համպդեն-Սիդնեյ քոլեջում լատիներենի մի դասախոս` ուսումնասիրելով Lorem Ipsum տեքստի ամենատարօրինակ բառերից"
;

const char * test_gz_string2 = "a";
const char * test_gz_string3 = "Hello world";
const char * test_gz_string4 = "Привет мир";
const char * test_gz_string5 = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum";
const char * test_gz_string6 = "Lorem Ipsum - это текст-рыба, часто используемый в печати и вэб-дизайне. Lorem Ipsum является стандартной \"рыбой\" для текстов на латинице с начала XVI века. В то время некий безымянный печатник создал большую коллекцию размеров и форм шрифтов, используя Lorem Ipsum для распечатки образцов. Lorem Ipsum не только успешно пережил без заметных изменений пять веков, но и перешагнул в электронный дизайн. Его популяризации в новое время послужили публикация листов Letraset с образцами Lorem Ipsum в 60-х годах и, в более недавнее время, программы электронной вёрстки типа Aldus PageMaker, в шаблонах которых используется Lorem Ipsum.";
const char * test_gz_string7 = "Lorem Ipsum ist ein einfacher Demo-Text für die Print- und Schriftindustrie. Lorem Ipsum ist in der Industrie bereits der Standard Demo-Text seit 1500, als ein unbekannter Schriftsteller eine Hand voll Wörter nahm und diese durcheinander warf um ein Musterbuch zu erstellen. Es hat nicht nur 5 Jahrhunderte überlebt, sondern auch in Spruch in die elektronische Schriftbearbeitung geschafft (bemerke, nahezu unverändert). Bekannt wurde es 1960, mit dem erscheinen von \"Letraset\", welches Passagen von Lorem Ipsum enhielt, so wie Desktop Software wie \"Aldus PageMaker\" - ebenfalls mit Lorem Ipsum.";
const char * test_gz_string8 = "Lorem Ipsum，也称乱数假文或者哑元文本， 是印刷及排版领域所常用的虚拟文字。由于曾经一台匿名的打印机刻意打乱了一盒印刷字体从而造出一本字体样品书，Lorem Ipsum从西元15世纪起就被作为此领域的标准文本使用。它不仅延续了五个世纪，还通过了电子排版的挑战，其雏形却依然保存至今。在1960年代，”Leatraset”公司发布了印刷着Lorem Ipsum段落的纸张，从而广泛普及了它的使用。最近，计算机桌面出版软件”Aldus PageMaker”也通过同样的方式使Lorem Ipsum落入大众的视野。";

int test_gz_creation_string(void) {
	const size_t strings_count = 8;
	const char * strings[strings_count] = {
		test_gz_string1,
		test_gz_string2,
		test_gz_string3,
		test_gz_string4,
		test_gz_string5,
		test_gz_string6,
		test_gz_string7,
		test_gz_string8
	};

	int i;
	const char * src_string;
	size_t src_string_len, compr_size, decompr_size;
	void * compr_buff;

	for (i = 0; i < strings_count; i++) {
		src_string = strings[i];
		src_string_len = strlen(src_string);
		assert(src_string_len > 0);

		compr_size = 0;
		compr_buff = nhr_gz_compress(src_string, src_string_len, &compr_size, NHR_GZ_METHOD_DEFLATE);
		assert(compr_buff);
		assert(compr_size > 0);

		decompr_size = 0;
		char * decompr_string = (char *)nhr_gz_decompress(compr_buff, compr_size, &decompr_size, NHR_GZ_METHOD_DEFLATE);

		assert(src_string_len == decompr_size);
		assert(strncmp(src_string, decompr_string, decompr_size) == 0);

		nhr_free(compr_buff);
		nhr_free(decompr_string);


		compr_size = 0;
		compr_buff = nhr_gz_compress(src_string, src_string_len, &compr_size, NHR_GZ_METHOD_GZIP);
		assert(compr_buff);
		assert(compr_size > 0);

		decompr_size = 0;
		decompr_string = (char *)nhr_gz_decompress(compr_buff, compr_size, &decompr_size, NHR_GZ_METHOD_GZIP);

		assert(src_string_len == decompr_size);
		assert(strncmp(src_string, decompr_string, decompr_size) == 0);

		nhr_free(compr_buff);
		nhr_free(decompr_string);
	}
	return 0;
}

int test_gz_big_data(void) {

	size_t mbts, src_size, dst_size;
	void * src, *compr_buff, *dst;

	for (mbts = 1; mbts < 10; mbts++) {
		src_size = mbts * 1024 * 1024;
		src = nhr_malloc(src_size); // any data inside

		size_t compr_size = 0;
		compr_buff = nhr_gz_compress(src, src_size, &compr_size, NHR_GZ_METHOD_DEFLATE);
		assert(compr_buff);
		assert(compr_size > 0);

		dst_size = 0;
		dst = nhr_gz_decompress(compr_buff, compr_size, &dst_size, NHR_GZ_METHOD_DEFLATE);

		assert(src_size == dst_size);
		assert(memcmp(src, dst, src_size) == 0);

		nhr_free(src);
		nhr_free(dst);


		src = nhr_malloc(src_size); // any data inside

		compr_size = 0;
		compr_buff = nhr_gz_compress(src, src_size, &compr_size, NHR_GZ_METHOD_GZIP);
		assert(compr_buff);
		assert(compr_size > 0);

		dst_size = 0;
		dst = nhr_gz_decompress(compr_buff, compr_size, &dst_size, NHR_GZ_METHOD_GZIP);

		assert(src_size == dst_size);
		assert(memcmp(src, dst, src_size) == 0);

		nhr_free(src);
		nhr_free(dst);
	}

	return 0;
}

int test_gz_creation(void) {

	int ret = test_gz_creation_string();
	assert(ret == 0);

	ret += test_gz_big_data();
	assert(ret == 0);

	return ret;
}
#endif

#if !defined(XCODE)
int main(int argc, char* argv[]) {

	int ret = 0;
#if defined(NHR_GZIP)
	ret += test_gz_creation();
	assert(ret == 0);
#endif

	return ret;
}
#endif
