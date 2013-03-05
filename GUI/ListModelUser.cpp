#include <Common/Global.h>
#include "Global.h"
#include "ListModelUser.h"
using namespace std;
using namespace Lightbox;

void ListModelUser::ensureIndexed() const
{
	if (m_dirty)
	{
		m_dirty = false;
		m_ids.clear();
		m_rids.clear();
		if (m_model)
		{
			m_ids = m_model->itemIds();
			for (unsigned i = 0; i < m_ids.size(); ++i)
				m_rids[m_ids[i]] = i;
		}
	}
}
