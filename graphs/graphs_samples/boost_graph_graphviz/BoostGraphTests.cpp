#include "stdafx.h"
#include "BoostGraph.h"
#include <sstream>
#include <boost/algorithm/string.hpp>


using std::vector;
using std::string;
using std::stringstream;


namespace
{

const char C_INPUT_1[] = R"***(
digraph G {
1 [root=1];
2;
3;
4;
5;
1->2 [weight=2];
2->3 [weight=3];
2->4 [weight=-3];
3->1 [weight=-4];
4->5 [weight=2];
}
)***";

const char C_OUTPUT_1[] = R"***(
0 0
2 1 2
5 2 2 3
-1 2 2 4
1 3 2 4 5
)***";

const char C_INPUT_2[] = R"***(
digraph G {
1 [root=1];
2;
3;
4;
1->2 [weight=3];
1->3 [weight=7];
2->3 [weight=2];
2->4 [weight=1];
3->2 [weight=2];
3->4 [weight=2];
4->3 [weight=2];
}
)***";

const char C_OUTPUT_2[] = R"***(
0 0
3 1 2
5 2 2 3
4 2 2 4
)***";

const char C_INPUT_3[] = R"***(
digraph G {
1 [root=1];
2;
3;
4;
1->2 [weight=3];
3->4 [weight=4];
}
)***";

const char C_OUTPUT_3[] = R"***(
0 0
3 1 2
No
No
)***";

const char C_INPUT_4[] = R"***(
digraph G {
1 [root=1];
2;
3;
4;
1->2 [weight=2];
2->3 [weight=4];
3->1 [weight=-9];
2->4 [weight=5];
}
)***";

const char C_OUTPUT_4[] = R"***(
No
4 2 3 1 2
)***";

// ���������� split ��� ������������� �������� � ��������� ����� ��� ��������.
vector<string> SplitWords(string const& text)
{
	std::string trimmed = boost::trim_copy(text);

	vector<string> words;
	boost::split(words, trimmed, boost::is_space(), boost::token_compress_on);
	return words;
}

}

BOOST_AUTO_TEST_SUITE(Graph_with_Bellmand_Ford_algorithm)
	BOOST_AUTO_TEST_CASE(finds_correct_shortest_paths)
	{
		CBoostGraph graph;
		stringstream input(C_INPUT_1);
		stringstream output;
		BOOST_CHECK(graph.ReadGraphviz(input));
		graph.RunBellmanFord(output);
		vector<string> words = SplitWords(output.str());
		vector<string> checkWords = SplitWords(C_OUTPUT_1);
		BOOST_CHECK(words == checkWords);
	}

	BOOST_AUTO_TEST_CASE(has_no_negative_loop_false_positives)
	{
		CBoostGraph graph;
		stringstream input(C_INPUT_2);
		stringstream output;
		BOOST_CHECK(graph.ReadGraphviz(input));
		graph.RunBellmanFord(output);
		vector<string> words = SplitWords(output.str());
		vector<string> checkWords = SplitWords(C_OUTPUT_2);
		BOOST_CHECK(words == checkWords);
	}

	BOOST_AUTO_TEST_CASE(prints_No_if_no_path)
	{
		CBoostGraph graph;
		stringstream input(C_INPUT_3);
		stringstream output;
		BOOST_CHECK(graph.ReadGraphviz(input));
		graph.RunBellmanFord(output);
		vector<string> words = SplitWords(output.str());
		vector<string> checkWords = SplitWords(C_OUTPUT_3);
		BOOST_CHECK(words == checkWords);
	}

	BOOST_AUTO_TEST_CASE(finds_negative_loop_and_prints_it)
	{
		CBoostGraph graph;
		stringstream input(C_INPUT_4);
		stringstream output;
		BOOST_CHECK(graph.ReadGraphviz(input));
		graph.RunBellmanFord(output);
		vector<string> words = SplitWords(output.str());
		vector<string> checkWords = SplitWords(C_OUTPUT_4);
		BOOST_CHECK(words == checkWords);
	}
BOOST_AUTO_TEST_SUITE_END()